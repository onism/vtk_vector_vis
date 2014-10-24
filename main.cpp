#include "mainwindow.h"
#include <QApplication>
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)
#include <vtk/vtkVersion.h>
#include <vtk/vtkArrowSource.h>
#include <vtk/vtkCellArray.h>
#include <vtk/vtkGlyph2D.h>
#include <vtk/vtkPointData.h>
#include <vtk/vtkImageData.h>
#include <vtk/vtkImageSliceMapper.h>
#include <vtk/vtkImageSlice.h>
#include <vtk/vtkInteractorStyleImage.h>
#include <vtk/vtkPolyDataMapper.h>
#include <vtk/vtkPolyData.h>
#include <vtk/vtkPoints.h>
#include <vtk/vtkRenderer.h>
#include <vtk/vtkRenderWindow.h>
#include <vtk/vtkRenderWindowInteractor.h>
#include <vtk/vtkSmartPointer.h>
#include <vtk/vtkXMLPolyDataWriter.h>

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
    // Create an image
     vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();

     // Specify the size of the image data
     image->SetDimensions(50,50,50);

   #if VTK_MAJOR_VERSION <= 5
     image->SetNumberOfScalarComponents(3);
     image->SetScalarTypeToFloat();
     image->AllocateScalars();
   #else
     image->AllocateScalars(VTK_FLOAT,3);
   #endif
     int* dims = image->GetDimensions();

     // Zero the image
     for (int y = 0; y < dims[1]; y++)
       {
       for (int x = 0; x < dims[0]; x++)
         {
             for (int z = 0; z < dims[2]; z++)
             {
         float* pixel = static_cast<float*>(image->GetScalarPointer(x,y,z));
         pixel[0] = 0.0;
         pixel[1] = 0.0;
         pixel[2] = 0.0;
             }
         }
       }

     {
     float* pixel = static_cast<float*>(image->GetScalarPointer(20,20,10));
     pixel[0] = -10.0;
     pixel[1] = 5.0;
     pixel[2] = 15.0;
     }
     {
     float* pixel = static_cast<float*>(image->GetScalarPointer(20,10,20));
     pixel[0] = -10.0;
     pixel[1] = 5.0;
     pixel[2] = 25.0;
     }
     {
     float* pixel = static_cast<float*>(image->GetScalarPointer(20,40,0));
     pixel[0] = -10.0;
     pixel[1] = 5.0;
     pixel[1] = 10.0;
     }

     {
     float* pixel = static_cast<float*>(image->GetScalarPointer(30,30,10));
     pixel[0] = 10.0;
     pixel[1] = 10.0;
     pixel[2] = 10.0;
     }

     // A better way to do this is (should be tested for compilablilty and correctness):
     // image->GetPointData()->SetActiveVectors(image->GetPointData()->GetScalars()->GetName().c_str());
     image->GetPointData()->SetActiveVectors("ImageScalars");

     // Setup the arrows
     vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
     arrowSource->Update();

     vtkSmartPointer<vtkGlyph2D> glyphFilter = vtkSmartPointer<vtkGlyph2D>::New();
     glyphFilter->SetSourceConnection(arrowSource->GetOutputPort());
     glyphFilter->OrientOn();
     glyphFilter->SetVectorModeToUseVector();
   #if VTK_MAJOR_VERSION <= 5
     glyphFilter->SetInputConnection(image->GetProducerPort());
   #else
     glyphFilter->SetInputData(image);
   #endif
     glyphFilter->Update();

     // Create actors
     vtkSmartPointer<vtkImageSliceMapper> imageMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
   #if VTK_MAJOR_VERSION <= 5
     imageMapper->SetInputConnection(image->GetProducerPort());
   #else
     imageMapper->SetInputData(image);
   #endif

     vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
     imageSlice->SetMapper(imageMapper);

     vtkSmartPointer<vtkPolyDataMapper> vectorMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
     vectorMapper->SetInputConnection(glyphFilter->GetOutputPort());
     vtkSmartPointer<vtkActor> vectorActor = vtkSmartPointer<vtkActor>::New();
     vectorActor->SetMapper(vectorMapper);

     // Setup renderer
     vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
     renderer->AddViewProp(imageSlice);
     renderer->AddViewProp(vectorActor);
     renderer->ResetCamera();

     // Setup render window
     vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
     renderWindow->AddRenderer(renderer);

     // Setup render window interactor
     vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
     vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
     //renderWindowInteractor->SetInteractorStyle(style);

     // Render and start interaction
     renderWindowInteractor->SetRenderWindow(renderWindow);
     renderWindowInteractor->Initialize();

     renderWindowInteractor->Start();
     return 1;
}
