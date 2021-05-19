/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#include "TrackMenCameraTrackingData.h"
#include "Controllers/LiveLinkTransformController.h"
#include "UTrackMenCameraController.generated.h"

class UCineCameraComponent;
class UMaterial;
class UMaterialInstanceDynamic;
class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;
class UTrackMenLiveLinkCameraControllerComponent;

/**
* Applies TrackMen LiveLink data to a CineCameraActor.
*/
UCLASS()
class UTrackMenCameraController : public ULiveLinkTransformController
{
	GENERATED_BODY()
public:
	UTrackMenCameraController();

	UPROPERTY(VisibleAnywhere, DisplayName = "Incoming camera static data", Category = "TrackMen")
		FTrackMenCameraStaticData TrackingStatic;
	UPROPERTY(VisibleAnywhere, DisplayName = "Incoming camera tracking data", Category = "TrackMen")
		FTrackMenCameraFrameData TrackingFrame;

	virtual void Tick(float DeltaTime, const FLiveLinkSubjectFrameData& SubjectData) override;
	virtual bool IsRoleSupported(const TSubclassOf<ULiveLinkRole>& RoleToSupport) override;
	virtual TSubclassOf<UActorComponent> GetDesiredComponentClass() const override;


private:
	void SaveSubjectDataToMembers(const FLiveLinkSubjectFrameData &SubjectData);
	void ApplyDataToActor();
	void ApplyTransformData();

	// UI / Controller Component functions
	UTrackMenLiveLinkCameraControllerComponent* GetControllerComponent();
	bool IsTransformEnabled();
	bool IsChipSizeEnabled();
	bool IsCenterShiftEnabled();
	bool IsLensDistortionEnabled();
	bool IsFocalLengthEnabled();
	bool IsApertureEnabled();
	bool IsFocusDistanceEnabled();

	// Lens model functions
	void ApplyLensData();
	void ApplyLensDataToCineCamera(UCineCameraComponent * camera, const float tex_coord_scale);
	void ApplyLensDataToMaterial(float &tex_coord_scale);

	// Asset functions
	void CheckForLensMaterialInstance(UCineCameraComponent* camera);
	void CheckForMaterialParameterCollectionInstance();
	void CreateLensMaterialInstance(UCineCameraComponent* camera);
	void FindLensDistortionMaterial();
	void FindMaterialParameterCollection();

	// Lens model related members
	UMaterial* m_lens_mat = nullptr;
	UMaterialInstanceDynamic* m_lens_mat_inst = nullptr;

	// Material parameter collection for Composure
	UMaterialParameterCollection* m_param_collection = nullptr;
	UMaterialParameterCollectionInstance* m_param_collection_inst = nullptr;
};
