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

#include "mitkPluginActivator.h"
#include "QmitkMITKIGTNavigationToolManagerView.h"
#include "QmitkMITKIGTTrackingToolboxView.h"
#include "QmitkNavigationDataPlayerView.h"
#include "QmitkIGTNavigationToolCalibration.h"
#include "QmitkIGTFiducialRegistration.h"

//#include <mitkPersistenceService.h> //Workaround for bug in persistence module (see bug 16643 for details)
                                    //CAN BE REMOVED WHEN THE BUG IS FIXED

namespace mitk {

  ctkPluginContext* PluginActivator::m_Context = nullptr;

void PluginActivator::start(ctkPluginContext* context)
{
 // mitk::PersistenceService::LoadModule(); //Workaround for bug in persistence module (see bug 16643 for details)
                                          //CAN BE REMOVED WHEN THE BUG IS FIXED
  m_Context = context;

  BERRY_REGISTER_EXTENSION_CLASS(QmitkMITKIGTNavigationToolManagerView, context)
  BERRY_REGISTER_EXTENSION_CLASS(QmitkMITKIGTTrackingToolboxView , context)
  BERRY_REGISTER_EXTENSION_CLASS(QmitkNavigationDataPlayerView , context)
  BERRY_REGISTER_EXTENSION_CLASS(QmitkIGTNavigationToolCalibration , context)
  BERRY_REGISTER_EXTENSION_CLASS(QmitkIGTFiducialRegistration, context)


}

void PluginActivator::stop(ctkPluginContext* context)
{
  m_Context = nullptr;

  Q_UNUSED(context)
}

ctkPluginContext *PluginActivator::GetContext()
{
  return m_Context;
}

}
