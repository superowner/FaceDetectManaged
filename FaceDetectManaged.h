// FaceDetectManaged.h

#pragma once
#include "facedetect-dll.h"
#pragma comment(lib,"libfacedetect.lib")

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Collections::Generic;

namespace FaceDetectManaged {

	public ref class FaceDetectManaged
	{
	public:
		static List<Rectangle>^ FrontalDetect(Bitmap^ bmp, float scale, int min_nb, int min_size, int max_size);
		static List<Rectangle>^ MultiviewDetect(Bitmap^ bmp, bool frontalPrior, float scale, int min_nb, int min_size, int max_size);
	private:
		static List<Rectangle>^ toList(int* arr);
		static unsigned char* toGray(Bitmap^ bmp, int& stride);
	};
}
