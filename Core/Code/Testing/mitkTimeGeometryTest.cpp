/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "mitkTimeGeometry.h"
#include "mitkGeometry3D.h"

#include "mitkRotationOperation.h"
#include "mitkInteractionConst.h"
#include <mitkMatrixConvert.h>
#include <mitkImageCast.h>

#include "mitkTestingMacros.h"
#include <fstream>
#include <mitkVector.h>


#include <mitkStandaloneDataStorage.h>
#include "mitkImageGenerator.h"
#include "mitkPointSet.h"
#include <limits>

class mitkTimeGeometryTestClass
{
public:
  void Translation_Image_MovedOrigin(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    // DimX, DimY, DimZ,
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::Geometry3D::Pointer geometry = baseData->GetTimeGeometry()->GetGeometryForTimeStep(0);
    mitk::Point3D imageOrigin = geometry->GetOrigin();
    mitk::Point3D expectedOrigin;
    expectedOrigin[0] = 0;
    expectedOrigin[1] = 0;
    expectedOrigin[2] = 0;
    MITK_TEST_CONDITION(mitk::Equal(imageOrigin, expectedOrigin), "Original origin match expected origin");

    expectedOrigin[0] = 0.325;
    expectedOrigin[1] = 0.487;
    expectedOrigin[2] = 0.78;

    mitk::Vector3D translationVector;
    translationVector[0] = expectedOrigin[0];
    translationVector[1] = expectedOrigin[1];
    translationVector[2] = expectedOrigin[2];

    for (mitk::TimeStepType timeStep = 0; timeStep < image->GetTimeGeometry()->GetNumberOfTimeSteps(); ++timeStep)
    {
      image->GetTimeGeometry()->GetGeometryForTimeStep(timeStep)->Translate(translationVector);
    }
    imageOrigin = image->GetGeometry(0)->GetOrigin();
    MITK_TEST_CONDITION(mitk::Equal(imageOrigin, expectedOrigin), "Translated origin match expected origin");

    expectedOrigin[0] = 2*translationVector[0];
    expectedOrigin[1] = 2*translationVector[1];
    expectedOrigin[2] = 2*translationVector[2];

    for (mitk::TimeStepType timeStep = 0; timeStep < image->GetTimeGeometry()->GetNumberOfTimeSteps(); ++timeStep)
    {
      image->GetTimeGeometry()->GetGeometryForTimeStep(timeStep)->Translate(translationVector);
    }
    imageOrigin = image->GetGeometry(0)->GetOrigin();
    MITK_TEST_CONDITION(mitk::Equal(imageOrigin, expectedOrigin), "Translated origin match expected origin");

  }


  void Rotate_Image_RotatedPoint(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::StandaloneDataStorage::Pointer ds = mitk::StandaloneDataStorage::New();
    mitk::DataNode::Pointer dataNode = mitk::DataNode::New();

    // DimX, DimY, DimZ,
    dataNode->SetData(baseData);
    ds->Add(dataNode);
    mitk::Geometry3D::Pointer geometry = baseData->GetTimeGeometry()->GetGeometryForTimeStep(0);
    mitk::Point3D expectedPoint;
    expectedPoint[0] = 3*0.5;
    expectedPoint[1] = 3*0.33;
    expectedPoint[2] = 3*0.78;
    mitk::Point3D originalPoint;
    originalPoint[0] = 3;
    originalPoint[1] = 3;
    originalPoint[2] = 3;
    mitk::Point3D worldPoint;
    geometry->IndexToWorld(originalPoint, worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Index-to-World without rotation as expected ");

    mitk::Point3D pointOfRotation;
    pointOfRotation[0] = 0;
    pointOfRotation[1] = 0;
    pointOfRotation[2] = 0;
    mitk::Vector3D vectorOfRotation;
    vectorOfRotation[0] = 1;
    vectorOfRotation[1] = 0.5;
    vectorOfRotation[2] = 0.2;
    float angleOfRotation = 73.0;
    mitk::RotationOperation* rotation = new mitk::RotationOperation(mitk::OpROTATE,pointOfRotation, vectorOfRotation, angleOfRotation);

    baseData->GetTimeGeometry()->ExecuteOperation(rotation);
    delete rotation;

    expectedPoint[0] = 2.6080379;
    expectedPoint[1] = -0.75265157;
    expectedPoint[2] = 1.1564401;

    baseData->GetGeometry(0)->IndexToWorld(originalPoint,worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Rotation returns expected values ");
  }

  void Scale_Image_ScaledPoint(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    // DimX, DimY, DimZ,
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::Geometry3D::Pointer geometry = image->GetTimeGeometry()->GetGeometryForTimeStep(0);
    mitk::Point3D expectedPoint;
    expectedPoint[0] = 3*0.5;
    expectedPoint[1] = 3*0.33;
    expectedPoint[2] = 3*0.78;
    mitk::Point3D originalPoint;
    originalPoint[0] = 3;
    originalPoint[1] = 3;
    originalPoint[2] = 3;
    mitk::Point3D worldPoint;
    geometry->IndexToWorld(originalPoint, worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Index-to-World with old Scaling as expected ");

    mitk::Vector3D newSpacing;
    newSpacing[0] = 2;
    newSpacing[1] = 1.254;
    newSpacing[2] = 0.224;
    image->SetSpacing(newSpacing);

    expectedPoint[0] = 3*2;
    expectedPoint[1] = 3*1.254;
    expectedPoint[2] = 3*0.224;

    image->GetGeometry(0)->IndexToWorld(originalPoint,worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Index-toWorld with new Scaling returns expected values ");
  }

  void GetMinimumTimePoint_4DImage_Zero(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::TimePointType expectedTimePoint = geometry->GetMinimumTimePoint();
    MITK_TEST_CONDITION(mitk::Equal(expectedTimePoint, 0), "Returns correct minimum time point ");
  }

    void GetMaximumTimePoint_4DImage_DimT(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::TimePointType expectedTimePoint = geometry->GetMaximumTimePoint();
    MITK_TEST_CONDITION(mitk::Equal(expectedTimePoint, DimT), "Returns correct maximum time point ");
  }

    void GetNumberOfTimeSteps_Image_ReturnDimT(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    mitk::TimeStepType expectedTimeSteps = geometry->GetNumberOfTimeSteps();
    MITK_TEST_CONDITION(mitk::Equal(expectedTimeSteps, DimT), "Returns correct number of time Steps ");
  }

    void GetMinimumTimePoint_3DImage_Min(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    mitk::TimePointType expectedTimePoint = geometry->GetMinimumTimePoint();
    MITK_TEST_CONDITION(mitk::Equal(expectedTimePoint, -std::numeric_limits<mitk::TimePointType>().max()), "Returns correct minimum time point ");
  }

    void GetMaximumTimePoint_3DImage_Max(mitk::BaseData* baseData,unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    mitk::TimePointType expectedTimePoint = geometry->GetMaximumTimePoint();
    MITK_INFO << expectedTimePoint;
    MITK_INFO << std::numeric_limits<mitk::TimePointType>().max();
    MITK_TEST_CONDITION(mitk::Equal(expectedTimePoint, std::numeric_limits<mitk::TimePointType>().max()), "Returns correct maximum time point ");
  }

  void GetTimeBounds_4DImage_ZeroAndDimT(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::TimeBounds expectedTimeBounds = geometry->GetTimeBounds();
    MITK_TEST_CONDITION(mitk::Equal(expectedTimeBounds[0], 0), "Returns correct minimum time point ");
    MITK_TEST_CONDITION(mitk::Equal(expectedTimeBounds[1], DimT), "Returns correct maximum time point ");
  }

  void GetTimeBounds_3DImage_ZeroAndDimT(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    mitk::TimeBounds expectedTimeBounds = geometry->GetTimeBounds();
    MITK_TEST_CONDITION(mitk::Equal(expectedTimeBounds[0], -std::numeric_limits<mitk::TimePointType>().max()), "Returns correct minimum time point ");
    MITK_TEST_CONDITION(mitk::Equal(expectedTimeBounds[1], std::numeric_limits<mitk::TimePointType>().max()), "Returns correct maximum time point ");
  }

  void IsValidTimePoint_ImageValidTimePoint_True(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    bool isValid = geometry->IsValidTimePoint(DimT-1);
    MITK_TEST_CONDITION(mitk::Equal(isValid, true), "Is valid time Point correct minimum time point ");
  }

  void IsValidTimePoint_ImageNegativInvalidTimePoint_False(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    bool isValid = geometry->IsValidTimePoint(-DimT);
    MITK_TEST_CONDITION(mitk::Equal(isValid, false), "Is invalid time Point correct minimum time point ");
  }

  void IsValidTimePoint_ImageInvalidTimePoint_False(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    bool isValid = geometry->IsValidTimePoint(DimT+1);
    MITK_TEST_CONDITION(mitk::Equal(isValid, false), "Is invalid time Point correct minimum time point ");
  }

  void IsValidTimeStep_ImageValidTimeStep_True(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    bool isValid = geometry->IsValidTimeStep(DimT-1);
    MITK_TEST_CONDITION(mitk::Equal(isValid, true), "Is valid time Point correct minimum time point ");
  }

  void IsValidTimeStep_ImageNegativInvalidTimeStep_False(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    bool isValid = geometry->IsValidTimeStep(-DimT);
    MITK_TEST_CONDITION(mitk::Equal(isValid, false), "Is invalid time Point correct minimum time point ");
  }

  void IsValidTimeStep_ImageInvalidTimeStep_False(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    bool isValid = geometry->IsValidTimeStep(DimT);
    MITK_TEST_CONDITION(mitk::Equal(isValid, false), "Is invalid time Point correct minimum time point ");
  }

  void TimeStepToTimePoint_ImageValidTimeStep_TimePoint(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    mitk::TimePointType timePoint= geometry->TimeStepToTimePoint(DimT-1);
    MITK_TEST_CONDITION(mitk::Equal(timePoint, DimT-1), "Calculated right time Point for Time Step ");
  }

  void TimeStepToTimePoint_ImageInvalidTimeStep_TimePoint(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    mitk::TimePointType timePoint= geometry->TimeStepToTimePoint(DimT+1);
    MITK_TEST_CONDITION(mitk::Equal(timePoint, DimT+1), "Calculated right time Point for invalid Time Step ");
  }

  void TimePointToTimeStep_ImageValidTimePoint_TimePoint(mitk::BaseData* baseData, unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::TimeGeometry::Pointer geometry = baseData->GetTimeGeometry();
    mitk::TimeStepType timePoint= geometry->TimePointToTimeStep(DimT-0.5);
    MITK_TEST_CONDITION(mitk::Equal(timePoint, DimT-1), "Calculated right time step for valid time point");
  }

  void TimePointToTimeStep_4DImageInvalidTimePoint_TimePoint(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::TimeStepType timePoint= geometry->TimePointToTimeStep(DimT+1.5);
    MITK_TEST_CONDITION(mitk::Equal(timePoint, DimT+1), "Calculated right time step for invalid time point");
  }

  void TimePointToTimeStep_4DImageNegativInvalidTimePoint_TimePoint(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::TimePointType negativTimePoint = (-1.0*DimT) - 1.5;
    mitk::TimeStepType timePoint= geometry->TimePointToTimeStep(negativTimePoint);
    MITK_TEST_CONDITION(mitk::Equal(timePoint, 0), "Calculated right time step for negativ invalid time point");
  }

  void GetGeometryForTimeStep_ImageValidTimeStep_CorrectGeometry(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryForTimeStep(DimT-1);
    MITK_TEST_CONDITION(geometry3D.IsNotNull(), "Non-zero geometry returned");

    mitk::Point3D expectedPoint;
    expectedPoint[0] = 3*0.5;
    expectedPoint[1] = 3*0.33;
    expectedPoint[2] = 3*0.78;
    mitk::Point3D originalPoint;
    originalPoint[0] = 3;
    originalPoint[1] = 3;
    originalPoint[2] = 3;
    mitk::Point3D worldPoint;
    geometry3D->IndexToWorld(originalPoint, worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Geometry transformation match expection. ");
  }

  void GetGeometryForTimeStep_ImageInvalidTimeStep_NullPointer(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryForTimeStep(DimT+1);
    MITK_TEST_CONDITION(geometry3D.IsNull(), "Null-Pointer geometry returned");
  }

  void GetGeometryForTimePoint_ImageValidTimePoint_CorrectGeometry(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryForTimePoint(DimT-0.5);
    MITK_TEST_CONDITION(geometry3D.IsNotNull(), "Non-zero geometry returned");

    mitk::Point3D expectedPoint;
    expectedPoint[0] = 3*0.5;
    expectedPoint[1] = 3*0.33;
    expectedPoint[2] = 3*0.78;
    mitk::Point3D originalPoint;
    originalPoint[0] = 3;
    originalPoint[1] = 3;
    originalPoint[2] = 3;
    mitk::Point3D worldPoint;
    geometry3D->IndexToWorld(originalPoint, worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Geometry transformation match expection. ");
  }

  void GetGeometryForTimePoint_4DImageInvalidTimePoint_NullPointer(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryForTimePoint(DimT+1);
    MITK_TEST_CONDITION(geometry3D.IsNull(), "Null-Pointer geometry returned with invalid time point");
  }

  void GetGeometryForTimePoint_4DImageNEgativInvalidTimePoint_NullPointer(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::TimePointType timePoint = (-1.0*(DimT)) -1;
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryForTimePoint(timePoint);
    MITK_TEST_CONDITION(geometry3D.IsNull(), "Null-Pointer geometry returned with invalid negativ time point");
  }

  void GetGeometryCloneForTimeStep_ImageValidTimeStep_CorrectGeometry(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryCloneForTimeStep(DimT-1);
    MITK_TEST_CONDITION(geometry3D.IsNotNull(), "Non-zero geometry returned");

    mitk::Point3D expectedPoint;
    expectedPoint[0] = 3*0.5;
    expectedPoint[1] = 3*0.33;
    expectedPoint[2] = 3*0.78;
    mitk::Point3D originalPoint;
    originalPoint[0] = 3;
    originalPoint[1] = 3;
    originalPoint[2] = 3;
    mitk::Point3D worldPoint;
    geometry3D->IndexToWorld(originalPoint, worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Geometry transformation match expection. ");

    mitk::Vector3D translationVector;
    translationVector[0] = 5;
    translationVector[1] = 8;
    translationVector[2] = 7;
    geometry3D->Translate(translationVector);

    geometry3D = geometry->GetGeometryForTimeStep(DimT-1);
    geometry3D->IndexToWorld(originalPoint, worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Geometry transformation not changed. ");
  }

  void GetGeometryCloneForTimeStep_ImageInvalidTimeStep_NullPointer(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryCloneForTimeStep(DimT+1);
    MITK_TEST_CONDITION(geometry3D.IsNull(), "Null-Pointer geometry returned");
  }

  void SetTimeStepGeometry_ImageValidTimeStep_CorrectGeometry(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryCloneForTimeStep(DimT-1);
    MITK_TEST_CONDITION(geometry3D.IsNotNull(), "Non-zero geometry returned");

    mitk::Vector3D translationVector;
    translationVector[0] = 5;
    translationVector[1] = 8;
    translationVector[2] = 7;
    geometry3D->Translate(translationVector);

    geometry->SetTimeStepGeometry(geometry3D,DimT-1);

    mitk::Point3D expectedPoint;
    expectedPoint[0] = 3*0.5+5;
    expectedPoint[1] = 3*0.33+8;
    expectedPoint[2] = 3*0.78+7;
    mitk::Point3D originalPoint;
    originalPoint[0] = 3;
    originalPoint[1] = 3;
    originalPoint[2] = 3;
    mitk::Point3D worldPoint;
    geometry->GetGeometryForTimeStep(DimT-1)->IndexToWorld(originalPoint, worldPoint);
    MITK_TEST_CONDITION(mitk::Equal(worldPoint, expectedPoint), "Geometry transformation match expection. ");
  }

  void Expand_ImageDoubleSize_SizeChanged(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();
    MITK_TEST_CONDITION(geometry->GetNumberOfTimeSteps()==DimT, "Number of time Steps match expection. ");

    geometry->Expand(DimT * 2);
    MITK_TEST_CONDITION(geometry->GetNumberOfTimeSteps()==DimT*2, "Number of time Steps match expection. ");

    mitk::Geometry3D::Pointer geometry3D = geometry->GetGeometryForTimeStep(DimT*2 -1);
    MITK_TEST_CONDITION(geometry3D.IsNotNull(), "Non-zero geometry is generated. ");
  }

  void CheckBounds_Image_PointsAsExpected(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();

    mitk::Point3D expectedPoint;

    expectedPoint[0] = -0.5*0.5;
    expectedPoint[1] = -0.5*0.33;
    expectedPoint[2] = -0.5*0.78;
    mitk::Point3D point =  geometry->GetCornerPointInWorld(0);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 0 as expected ");
    point =  geometry->GetCornerPointInWorld(true,true,true);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 0 as expected ");

    point =  geometry->GetCornerPointInWorld(1);
    expectedPoint[0] = -0.5*0.5;
    expectedPoint[1] = -0.5*0.33;
    expectedPoint[2] = 19.5*0.78;
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "GBounding Point 1 as expected ");
    point =  geometry->GetCornerPointInWorld(true,true,false);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 1 as expected ");

    point =  geometry->GetCornerPointInWorld(2);
    expectedPoint[0] = -0.5*0.5;
    expectedPoint[1] = 24.5*0.33;
    expectedPoint[2] = -0.5*0.78;
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 2 as expected ");
    point =  geometry->GetCornerPointInWorld(true,false,true);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 2 as expected ");

    point =  geometry->GetCornerPointInWorld(3);
    expectedPoint[0] = -0.5*0.5;
    expectedPoint[1] = 24.5*0.33;
    expectedPoint[2] = 19.5*0.78;
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 3 as expected  ");
    point =  geometry->GetCornerPointInWorld(true,false,false);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 3 as expected ");

    point =  geometry->GetCornerPointInWorld(4);
    expectedPoint[0] = 29.5*0.5;
    expectedPoint[1] = -0.5*0.33;
    expectedPoint[2] = -0.5*0.78;
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 4 as expected  ");
    point =  geometry->GetCornerPointInWorld(false,true,true);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 4 as expected ");

    point =  geometry->GetCornerPointInWorld(5);
    expectedPoint[0] = 29.5*0.5;
    expectedPoint[1] = -0.5*0.33;
    expectedPoint[2] = 19.5*0.78;
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 5 as expected  ");
    point =  geometry->GetCornerPointInWorld(false,true,false);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 5 as expected ");

    point =  geometry->GetCornerPointInWorld(6);
    expectedPoint[0] = 29.5*0.5;
    expectedPoint[1] = 24.5*0.33;
    expectedPoint[2] = -0.5*0.78;
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 6 as expected  ");
    point =  geometry->GetCornerPointInWorld(false,false,true);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 6 as expected ");

    point =  geometry->GetCornerPointInWorld(7);
    expectedPoint[0] = 29.5*0.5;
    expectedPoint[1] = 24.5*0.33;
    expectedPoint[2] = 19.5*0.78;
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 7 as expected  ");
    point =  geometry->GetCornerPointInWorld(false,false,false);
    MITK_TEST_CONDITION(mitk::Equal(expectedPoint, point), "Bounding Point 7 as expected ");
  }

  void CheckLength_Image_AsExpected(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();

    double dimension = geometry->GetDiagonalLengthInWorld();
    MITK_TEST_CONDITION(mitk::Equal(dimension,23.160796233014466 ), "Length  as expected ");

    dimension = geometry->GetDiagonalLength2InWorld();
    MITK_TEST_CONDITION(mitk::Equal(dimension,536.42248214721712 ), "Square length as expected ");
  }

  void CheckPointInside_ImagePointInside_True(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();

    mitk::Point3D expectedPoint;

    expectedPoint[0] = 10;
    expectedPoint[1] = 5;
    expectedPoint[2] = 5;
    bool isInside =  geometry->IsWorldPointInside(expectedPoint);
    MITK_TEST_CONDITION(isInside, "Point is inside Image...");
  }

  void CheckPointInside_ImagePointOutside_False(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();

    mitk::Point3D expectedPoint;

    expectedPoint[0] = 100;
    expectedPoint[1] = 500;
    expectedPoint[2] = 500;
    bool isInside =  geometry->IsWorldPointInside(expectedPoint);
    MITK_TEST_CONDITION(!isInside, "Point is outside Image...");
  }

  void CheckBounds_Image_AsSet(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();

    mitk::BoundingBox::BoundsArrayType bound =  geometry->GetBoundsInWorld();
    bool isEqual = true;
    isEqual = isEqual && mitk::Equal(bound[0], -0.5*0.5);
    isEqual = isEqual && mitk::Equal(bound[1], 29.5*0.5);
    isEqual = isEqual && mitk::Equal(bound[2], -0.5*0.33);
    isEqual = isEqual && mitk::Equal(bound[3], 24.5*0.33);
    isEqual = isEqual && mitk::Equal(bound[4], -0.5*0.78);
    isEqual = isEqual && mitk::Equal(bound[5], 19.5*0.78);

    MITK_TEST_CONDITION(isEqual, "Bounds as precalculated...");
  }

  void CheckExtent_Image_AsSet(unsigned int DimX, unsigned int DimY, unsigned int DimZ, unsigned int DimT)
  {
    mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(DimX, DimY, DimZ, DimT,0.5,0.33,0.78,100);
    mitk::TimeGeometry::Pointer geometry = image->GetTimeGeometry();

    mitk::BoundingBox::BoundsArrayType bound =  geometry->GetBoundsInWorld();
    bool isEqual = true;
    isEqual = isEqual && mitk::Equal(geometry->GetExtentInWorld(0), 30*0.5);
    isEqual = isEqual && mitk::Equal(geometry->GetExtentInWorld(1), 25*0.33);
    isEqual = isEqual && mitk::Equal(geometry->GetExtentInWorld(2), 20*0.78);

    MITK_TEST_CONDITION(isEqual, "Extent as precalculated...");
  }
};







int mitkTimeGeometryTest(int /*argc*/, char* /*argv*/[])
{
  MITK_TEST_BEGIN(mitkTimeGeometryTest);

  mitkTimeGeometryTestClass testClass;

  MITK_TEST_OUTPUT(<< "Test for 3D image");
  mitk::Image::Pointer image = mitk::ImageGenerator::GenerateRandomImage<float>(30, 25, 20, 1,0.5,0.33,0.78,100);
  testClass.Translation_Image_MovedOrigin(image->Clone(),30,25,20,1);
  testClass.Rotate_Image_RotatedPoint(image->Clone(),30,25,20,1);
  testClass.Scale_Image_ScaledPoint(30,25,20,1);
  testClass.GetNumberOfTimeSteps_Image_ReturnDimT(image->Clone(),30,25,20,1);
  testClass.GetMinimumTimePoint_3DImage_Min(image->Clone(),30,25,20,1);
  testClass.GetMaximumTimePoint_3DImage_Max(image->Clone(),30,25,20,1);
  testClass.GetTimeBounds_3DImage_ZeroAndDimT(image->Clone(),30,25,20,1);
  testClass.IsValidTimePoint_ImageValidTimePoint_True(image->Clone(),30,25,20,1);
  testClass.IsValidTimeStep_ImageValidTimeStep_True(image->Clone(), 30,25,20,1);
  testClass.IsValidTimeStep_ImageNegativInvalidTimeStep_False(image->Clone(), 30,25,20,1);
  testClass.IsValidTimeStep_ImageInvalidTimeStep_False(image->Clone(), 30,25,20,1);
  testClass.TimeStepToTimePoint_ImageValidTimeStep_TimePoint(image->Clone(), 30,25,20,1);
  testClass.TimeStepToTimePoint_ImageInvalidTimeStep_TimePoint(image->Clone(), 30,25,20,1);
  testClass.TimePointToTimeStep_ImageValidTimePoint_TimePoint(image->Clone(), 30,25,20,1);
  testClass.GetGeometryForTimeStep_ImageValidTimeStep_CorrectGeometry(30,25,20,1);
  testClass.GetGeometryForTimeStep_ImageInvalidTimeStep_NullPointer(30,25,20,1);
  testClass.GetGeometryForTimePoint_ImageValidTimePoint_CorrectGeometry(30,25,20,1);
  testClass.GetGeometryCloneForTimeStep_ImageValidTimeStep_CorrectGeometry(30,25,20,1);
  testClass.GetGeometryCloneForTimeStep_ImageInvalidTimeStep_NullPointer(30,25,20,1);
  testClass.SetTimeStepGeometry_ImageValidTimeStep_CorrectGeometry(30,25,20,1);
  testClass.Expand_ImageDoubleSize_SizeChanged(30,25,20,1);
  testClass.CheckBounds_Image_PointsAsExpected(30,25,20,1);
  testClass.CheckLength_Image_AsExpected(30,25,20,1);
  testClass.CheckPointInside_ImagePointInside_True(30,25,20,1);
  testClass.CheckPointInside_ImagePointOutside_False(30,25,20,1);
  testClass.CheckBounds_Image_AsSet(30,25,20,1);
  testClass.CheckExtent_Image_AsSet(30,25,20,1);

/*

  MITK_TEST_OUTPUT(<< "Test for 2D image");
  testClass.Translation_Image_MovedOrigin(30,25,1 ,1); // Test with 2D-Image
  testClass.Rotate_Image_RotatedPoint(30,25,1 ,1); // Test with 2D-Image
  testClass.Scale_Image_ScaledPoint(30,25,1 ,1); // Test with 2D-Image

*/

  MITK_TEST_OUTPUT(<< "Test for 3D+time image");
  image = mitk::ImageGenerator::GenerateRandomImage<float>(30, 25, 20, 5,0.5,0.33,0.78,100);
  testClass.Translation_Image_MovedOrigin(image->Clone(),30,25,20,5); // Test with 3D+t-Image
  testClass.Rotate_Image_RotatedPoint(image->Clone(),30,25,20,5); // Test with 3D+t-Image
  testClass.Scale_Image_ScaledPoint(30,25,20,5); // Test with 3D+t-Image
  testClass.GetNumberOfTimeSteps_Image_ReturnDimT(image->Clone(),30,25,20,5);
  testClass.GetMinimumTimePoint_4DImage_Zero(30,25,20,5);
  testClass.GetMaximumTimePoint_4DImage_DimT(30,25,20,5);
  testClass.GetTimeBounds_4DImage_ZeroAndDimT(30,25,20,5);
  testClass.IsValidTimePoint_ImageValidTimePoint_True(image->Clone(),30,25,20,5);
  testClass.IsValidTimePoint_ImageNegativInvalidTimePoint_False(30,25,20,5);
  testClass.IsValidTimePoint_ImageInvalidTimePoint_False(30,25,20,5);
  testClass.IsValidTimeStep_ImageValidTimeStep_True(image->Clone(), 30,25,20,5);
  testClass.IsValidTimeStep_ImageNegativInvalidTimeStep_False(image->Clone(), 30,25,20,5);
  testClass.IsValidTimeStep_ImageInvalidTimeStep_False(image->Clone(), 30,25,20,5);
  testClass.TimeStepToTimePoint_ImageValidTimeStep_TimePoint(image->Clone(), 30,25,20,5);
  testClass.TimeStepToTimePoint_ImageInvalidTimeStep_TimePoint(image->Clone(), 30,25,20,5);
  testClass.TimePointToTimeStep_ImageValidTimePoint_TimePoint(image->Clone(), 30,25,20,5);
  testClass.TimePointToTimeStep_4DImageInvalidTimePoint_TimePoint(30,25,20,5);
  testClass.TimePointToTimeStep_4DImageNegativInvalidTimePoint_TimePoint(30,25,20,5);
  testClass.GetGeometryForTimeStep_ImageValidTimeStep_CorrectGeometry(30,25,20,5);
  testClass.GetGeometryForTimeStep_ImageInvalidTimeStep_NullPointer(30,25,20,5);
  testClass.GetGeometryForTimePoint_ImageValidTimePoint_CorrectGeometry(30,25,20,5);
  testClass.GetGeometryForTimePoint_4DImageInvalidTimePoint_NullPointer(30,25,20,5);
  testClass.GetGeometryForTimePoint_4DImageNEgativInvalidTimePoint_NullPointer(30,25,20,5);
  testClass.GetGeometryCloneForTimeStep_ImageValidTimeStep_CorrectGeometry(30,25,20,5);
  testClass.GetGeometryCloneForTimeStep_ImageInvalidTimeStep_NullPointer(30,25,20,5);
  testClass.SetTimeStepGeometry_ImageValidTimeStep_CorrectGeometry(30,25,20,5);
  testClass.Expand_ImageDoubleSize_SizeChanged(30,25,20,5);
  testClass.CheckBounds_Image_PointsAsExpected(30,25,20,5);
  testClass.CheckLength_Image_AsExpected(30,25,20,5);
  testClass.CheckPointInside_ImagePointInside_True(30,25,20,5);
  testClass.CheckPointInside_ImagePointOutside_False(30,25,20,5);
  testClass.CheckBounds_Image_AsSet(30,25,20,5);
  testClass.CheckExtent_Image_AsSet(30,25,20,5);

/*

  MITK_TEST_OUTPUT(<< "Test for 2D+time image");
  testClass.Translation_Image_MovedOrigin(30,25,1 ,5); // Test with 2D+t-Image
  testClass.Rotate_Image_RotatedPoint(30,25,1 ,5); // Test with 2D+t-Image
  testClass.Scale_Image_ScaledPoint(30,25,1 ,5); // Test with 2D+t-Image

*/


  mitk::PointSet::Pointer pointSet = mitk::PointSet::New();
  mitk::Point3D pointA, pointB, pointC;
  pointA.Fill(1);
  pointB.Fill(2);
  pointC.Fill(3);
  pointSet->SetPoint(1,pointA);
  pointSet->SetPoint(2,pointB);
  pointSet->SetPoint(3,pointC);
  testClass.Translation_Image_MovedOrigin(pointSet->Clone(),30,25,20,1);
  //testClass.Rotate_Image_RotatedPoint(pointSet->Clone(),30,25,20,1);
  //testClass.Scale_Image_ScaledPoint(30,25,20,1);
  testClass.GetNumberOfTimeSteps_Image_ReturnDimT(pointSet->Clone(),30,25,20,1);
  testClass.GetMinimumTimePoint_3DImage_Min(pointSet->Clone(),30,25,20,1);
  testClass.GetMaximumTimePoint_3DImage_Max(pointSet->Clone(),30,25,20,1);
  testClass.GetTimeBounds_3DImage_ZeroAndDimT(pointSet->Clone(),30,25,20,1);
  testClass.IsValidTimePoint_ImageValidTimePoint_True(pointSet->Clone(),30,25,20,1);
  testClass.IsValidTimeStep_ImageValidTimeStep_True(pointSet->Clone(),30,25,20,1);
  testClass.IsValidTimeStep_ImageNegativInvalidTimeStep_False(pointSet->Clone(),30,25,20,1);
  testClass.IsValidTimeStep_ImageInvalidTimeStep_False(pointSet->Clone(),30,25,20,1);
  testClass.TimeStepToTimePoint_ImageValidTimeStep_TimePoint(pointSet->Clone(),30,25,20,1);
  testClass.TimeStepToTimePoint_ImageInvalidTimeStep_TimePoint(pointSet->Clone(),30,25,20,1);
  testClass.TimePointToTimeStep_ImageValidTimePoint_TimePoint(pointSet->Clone(),30,25,20,1);
  //testClass.GetGeometryForTimeStep_ImageValidTimeStep_CorrectGeometry(30,25,20,1);
  //testClass.GetGeometryForTimeStep_ImageInvalidTimeStep_NullPointer(30,25,20,1);
  //testClass.GetGeometryForTimePoint_ImageValidTimePoint_CorrectGeometry(30,25,20,1);
  //testClass.GetGeometryCloneForTimeStep_ImageValidTimeStep_CorrectGeometry(30,25,20,1);
  //testClass.GetGeometryCloneForTimeStep_ImageInvalidTimeStep_NullPointer(30,25,20,1);
  //testClass.SetTimeStepGeometry_ImageValidTimeStep_CorrectGeometry(30,25,20,1);
  //testClass.Expand_ImageDoubleSize_SizeChanged(30,25,20,1);
  //testClass.CheckBounds_Image_PointsAsExpected(30,25,20,1);
  //testClass.CheckLength_Image_AsExpected(30,25,20,1);
  //testClass.CheckPointInside_ImagePointInside_True(30,25,20,1);
  //testClass.CheckPointInside_ImagePointOutside_False(30,25,20,1);
  //testClass.CheckBounds_Image_AsSet(30,25,20,1);
  //testClass.CheckExtent_Image_AsSet(30,25,20,1);

  MITK_TEST_END();


  return EXIT_SUCCESS;
}
