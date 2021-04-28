/* Copyright 2020 TrackMen GmbH <mail@trackmen.de>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "UTrackMenCameraController.h"
#include "UTrackMenCameraRole.h"
#include "Components/SceneComponent.h"
#include "CineCameraComponent.h"
#include "Features/IModularFeatures.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "UObject/ConstructorHelpers.h"
#if WITH_EDITOR
#include "Kismet2/ComponentEditorUtils.h"
#endif

UTrackMenCameraController::UTrackMenCameraController() {
	FindLensDistortionMaterial();
	FindMaterialParameterCollection();
}

void UTrackMenCameraController::Tick(float DeltaTime, const FLiveLinkSubjectFrameData& SubjectData)
{
	SaveSubjectDataToMembers(SubjectData);
	ApplyDataToActor();
}

bool UTrackMenCameraController::IsRoleSupported(const TSubclassOf<ULiveLinkRole>& RoleToSupport)
{
	return RoleToSupport == UTrackMenCameraRole::StaticClass();
}

TSubclassOf<UActorComponent> UTrackMenCameraController::GetDesiredComponentClass() const
{
	return UCineCameraComponent::StaticClass();
}

void UTrackMenCameraController::SaveSubjectDataToMembers(const FLiveLinkSubjectFrameData &SubjectData)
{
	const FTrackMenCameraStaticData* StaticData = SubjectData.StaticData.Cast<FTrackMenCameraStaticData>();
	const FTrackMenCameraFrameData* FrameData = SubjectData.FrameData.Cast<FTrackMenCameraFrameData>();

	// Copy data because these properties shall be accessible in the editor later.
	if (StaticData != nullptr) {
		TrackingStatic = *StaticData;
	}
	if (FrameData != nullptr) {
		TrackingFrame = *FrameData;
	}
}

void UTrackMenCameraController::ApplyDataToActor()
{
	ApplyTransformData();
	ApplyLensData();
}

void UTrackMenCameraController::ApplyTransformData()
{
	USceneComponent* node = Cast<USceneComponent>(AttachedComponent);
	if (nullptr != node)
	{
		TransformData.ApplyTransform(node, TrackingFrame.Transform);
	}
}

void UTrackMenCameraController::ApplyLensData()
{
	auto camera = Cast<UCineCameraComponent>(AttachedComponent);
	if (!camera) {
		return;
	}

	CheckForLensMaterialInstance(camera);
	CheckForMaterialParameterCollectionInstance();

	auto tex_coord_scale = camera->PostProcessSettings.ScreenPercentage / 100.f;
	ApplyLensDataToCineCamera(camera, tex_coord_scale);
	ApplyLensDataToMaterial(tex_coord_scale);
}

void UTrackMenCameraController::CheckForMaterialParameterCollectionInstance()
{
	if (m_param_collection_inst == nullptr && m_param_collection != nullptr) {
		m_param_collection_inst = GetOuterActor()->GetWorld()->GetParameterCollectionInstance(m_param_collection);
	}
}

void UTrackMenCameraController::ApplyLensDataToCineCamera(UCineCameraComponent * camera, const float tex_coord_scale)
{
	auto lens_settings = camera->LensSettings;
	lens_settings.MinFocalLength = 0.01f;
	camera->LensSettings = lens_settings;

	// Need to adjust for screen percentage / overscan,
	// i.e rendering a larger image to avoid cut off edges
	// due to inward lens distortion.
	// 
	//                b'
	//             b
	// ----------|---|---|--
	//           |.)/   /
	//          a| /c  /
	//           |/   /
	//       a'  o   /c'
	//           |  /
	//           | /
	//       _   |/
	//           o'
	//
	//       a/b = a'/b'
	//  <=>  a' = a * b/b'
	//  <=>  a' = a * (1/tex_coord_scale)
	//
	//  o  = regular camera position
	//  o  = displaced camera position
	//  a  = focal length
	//  a' = focal length of displaced camera
	//  b  = half width/height of usual screen
	//  b' = half width/height of overscanned screen
	//
	// E.g. Twice the screen diameter (tex_coord_scale=2) 
	// needs half the focal length
	camera->CurrentFocalLength = TrackingFrame.FocalLength / tex_coord_scale;

	auto focus_settings = camera->FocusSettings;
	focus_settings.ManualFocusDistance = TrackingFrame.FocusDistance;
	camera->FocusSettings = focus_settings;

	FCameraFilmbackSettings fbsettings = camera->Filmback;
	fbsettings.SensorWidth = TrackingFrame.chip_size.X;
	fbsettings.SensorHeight = TrackingFrame.chip_size.Y;
	camera->Filmback = fbsettings;
}

void UTrackMenCameraController::ApplyLensDataToMaterial(float &tex_coord_scale)
{
	if (m_lens_mat_inst == nullptr) {
		return;
	}
	m_lens_mat_inst->SetScalarParameterValue("TexCoordScale", tex_coord_scale);
	m_lens_mat_inst->SetScalarParameterValue("k1", TrackingFrame.lens_distortion.X);
	m_lens_mat_inst->SetScalarParameterValue("k2", TrackingFrame.lens_distortion.Y);
	m_lens_mat_inst->SetScalarParameterValue("CenterX", TrackingFrame.center_shift.X);
	m_lens_mat_inst->SetScalarParameterValue("CenterY", TrackingFrame.center_shift.Y);
	m_lens_mat_inst->SetScalarParameterValue("ChipSizeX", TrackingFrame.chip_size.X);
	m_lens_mat_inst->SetScalarParameterValue("ChipSizeY", TrackingFrame.chip_size.Y);

	if (m_param_collection_inst) {
		m_param_collection_inst->SetScalarParameterValue(FName("TexCoordScale"), tex_coord_scale);
		m_param_collection_inst->SetScalarParameterValue(FName("k1"), TrackingFrame.lens_distortion.X);
		m_param_collection_inst->SetScalarParameterValue(FName("k2"), TrackingFrame.lens_distortion.Y);
		m_param_collection_inst->SetScalarParameterValue(FName("CenterX"), TrackingFrame.center_shift.X);
		m_param_collection_inst->SetScalarParameterValue(FName("CenterY"), TrackingFrame.center_shift.Y);
		m_param_collection_inst->SetScalarParameterValue(FName("ChipSizeX"), TrackingFrame.chip_size.X);
		m_param_collection_inst->SetScalarParameterValue(FName("ChipSizeY"), TrackingFrame.chip_size.Y);
	}
}

void UTrackMenCameraController::CheckForLensMaterialInstance(UCineCameraComponent* camera) {
	bool material_instance_found = false;

	// Check if a lens material is already present.
	for (FWeightedBlendable& blendable : camera->PostProcessSettings.WeightedBlendables.Array) {
		UMaterialInstanceDynamic* blendable_interface = Cast<UMaterialInstanceDynamic>(blendable.Object);
		if (blendable_interface && blendable_interface->Parent == m_lens_mat) {
			material_instance_found = true;
			m_lens_mat_inst = blendable_interface;
		}
	}

	// Create material if it does not exist yet.
	if (!material_instance_found) {
		CreateLensMaterialInstance(camera);
	}
}

void UTrackMenCameraController::CreateLensMaterialInstance(UCineCameraComponent* camera) {
	UE_LOG(LogTrackMenPlugin, Display, TEXT("CreateLensMaterialInstance"));
	m_lens_mat_inst = UMaterialInstanceDynamic::Create(m_lens_mat, this);
	camera->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.f, m_lens_mat_inst));
}

void UTrackMenCameraController::FindLensDistortionMaterial() {

	UE_LOG(LogTrackMenPlugin, Display, TEXT("FindLensDistortionMaterial"));

	static ConstructorHelpers::FObjectFinder<UMaterial> lens_dist_mat(
		TEXT("Material'/TrackMenVPCam/TrackMenLensDistortion.TrackMenLensDistortion'"));
	if (lens_dist_mat.Object) {
		m_lens_mat = (UMaterial*)lens_dist_mat.Object;
	}
}

void UTrackMenCameraController::FindMaterialParameterCollection() {

	UE_LOG(LogTrackMenPlugin, Display, TEXT("FindLensDistortionMaterialParameterCollection"));

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> mat_param_collection(
		TEXT("MaterialParameterCollection'/TrackMenVPCam/TrackMenLensDistortionMaterialParameterCollection.TrackMenLensDistortionMaterialParameterCollection'"));
	if (mat_param_collection.Object) {
		m_param_collection = (UMaterialParameterCollection*)mat_param_collection.Object;
	}
}
