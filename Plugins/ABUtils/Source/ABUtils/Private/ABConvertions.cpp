#include "ABConvertions.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

FVector2D UABConvertions::WorldToScreenPoint(const FVector& WorldLocation, const USceneCaptureComponent2D* SceneCapture)
{
    FTransform CameraTransform = SceneCapture->GetComponentTransform();
    FMatrix ViewMatrix = CameraTransform.ToInverseMatrixWithScale();

    float FOV = SceneCapture->FOVAngle;
    float AspectRatio = SceneCapture->TextureTarget->SizeX / (float)SceneCapture->TextureTarget->SizeY;
    float Near = GNearClippingPlane;
    float Far = 10000.0f;

    FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
        FOV * (float)PI / 360.0f, // HFOV in radians / 2
        AspectRatio,
        Near,
        Far
    );

    // Compute ViewProjection matrix
    FMatrix ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;

    // Transform world position
    FVector4 ClipSpace = ViewProjectionMatrix.TransformFVector4(FVector4(WorldLocation, 1.0f));

    // Perspective divide
    FVector2D NDC;
    NDC.X = ClipSpace.X / ClipSpace.W;
    NDC.Y = ClipSpace.Y / ClipSpace.W;

    // Convert to screen space (0 to 1)
    FVector2D ScreenUV;
    ScreenUV.X = (NDC.X + 1.0f) / 2.0f;
    ScreenUV.Y = (1.0f - NDC.Y) / 2.0f; // Invert Y

    // Convert to pixel coordinates (if needed)
    FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
    FVector2D ScreenPos = ScreenUV * ViewportSize;
}
