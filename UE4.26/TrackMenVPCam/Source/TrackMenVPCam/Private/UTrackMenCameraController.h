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

#include "TrackMenCameraTrackingData.h"
#include "Controllers/LiveLinkTransformController.h"
#include "UTrackMenCameraController.generated.h"

class UCineCameraComponent;
class UMaterial;
class UMaterialInstanceDynamic;
class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

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
