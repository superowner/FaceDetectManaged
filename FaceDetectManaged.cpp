// 这是主 DLL 文件。

#include "stdafx.h"

#include "FaceDetectManaged.h"

List<Rectangle>^ FaceDetectManaged::FaceDetectManaged::FrontalDetect(Bitmap^ bmp, float scale, int min_nb, int min_size, int max_size)
{
	int stride;
	unsigned char* buf = toGray(bmp, stride);
	List<Rectangle> ^r = toList(facedetect_frontal(buf, bmp->Width, bmp->Height, stride, scale, min_nb, min_size, max_size));
	return r;
}

List<Rectangle>^ FaceDetectManaged::FaceDetectManaged::MultiviewDetect(Bitmap^ bmp, float scale, int min_nb, int min_size, int max_size)
{
	int stride;
	unsigned char* buf = toGray(bmp, stride);
	List<Rectangle> ^r = toList(facedetect_multiview(buf, bmp->Width, bmp->Height, stride, scale, min_nb, min_size, max_size));
	return r;
}

List<Rectangle>^ FaceDetectManaged::FaceDetectManaged::toList(int* arr)
{
	List<Rectangle>^ res = gcnew List<Rectangle>();
	if (!arr) return res;
	for (int i = 0; i < *arr; ++i) {
        short * p = ((short*)(arr+1))+6*i;
		int x = p[0];
		int y = p[1];
		int w = p[2];
		int h = p[3];
		int neighbors = p[4];
		res->Add(Rectangle(x, y, w, h));
	}
	return res;
}

unsigned char* FaceDetectManaged::FaceDetectManaged::toGray(Bitmap^ bmp, int& stride)
{
	BitmapData^ bmpd;
	if (bmp->PixelFormat == PixelFormat::Format8bppIndexed) {
		stride = bmpd->Stride;
		bmpd = bmp->LockBits(Rectangle(0, 0, bmp->Width, bmp->Height), ImageLockMode::ReadOnly, bmp->PixelFormat);
		return (unsigned char*)bmpd->Scan0.ToPointer();
	}
	
	bmpd = bmp->LockBits(Rectangle(0, 0, bmp->Width, bmp->Height), ImageLockMode::ReadOnly, PixelFormat::Format24bppRgb);
	stride = bmp->Width + (4 - bmp->Width % 4) % 4;
	unsigned char* res = new unsigned char[stride * bmp->Height];
	unsigned char* pBmp = (unsigned char*)bmpd->Scan0.ToPointer();
	for (int offset = 0, y = 0; y < bmp->Height; ++y, offset += bmpd->Stride)
		for (int x = 0; x < bmpd->Width; ++x) {
			int b = pBmp[offset + x * 3 + 0],
				g = pBmp[offset + x * 3 + 1],
				r = pBmp[offset + x * 3 + 2];
			res[y * stride + x] = (unsigned char)((float)r * 0.3f + (float)g * 0.59f + (float)b * 0.11f);
		}
	bmp->UnlockBits(bmpd);
	return res;
}
