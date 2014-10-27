/*********************************
 * PropertyViewer.cpp
 * Connor Hilarides
 * Created 2014/10/27
 *********************************/

#include "Stdafx.h"
#include "PropertyViewer.h"

#using "GamePropertyViewer.dll"
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#using <PresentationFramework.dll>

using namespace GamePropertyViewer;

PropertyViewer ^*propertyWindow;

extern "C" void OpenPropertyViewer()
{
  propertyWindow = reinterpret_cast<decltype(propertyWindow)>(operator new(sizeof(PropertyViewer^)));
  *propertyWindow = gcnew PropertyViewer();
}

extern "C" void ClosePropertyViewer()
{
  System::Windows::Window ^wind = *propertyWindow;
  (wind);

  delete propertyWindow;
}
