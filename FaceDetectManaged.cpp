// 这是主 DLL 文件。

#include "stdafx.h"

#include "FaceDetectManaged.h"

List<Rectangle>^ FaceDetectManaged::FaceDetectManaged::FrontalDetect(Bitmap^ bmp, float scale, int min_nb, int min_size, int max_size)
{
	int stride;
	unsigned char* buf = toGray(bmp, stride);
	List<Rectangle> ^r = toList(facedetect_frontal(buf, bmp->Width, bmp->Height, stride, scale, min_nb, min_size, max_size));
	delete[] buf;
	return r;
}

List<Rectangle>^ FaceDetectManaged::FaceDetectManaged::MultiviewDetect(Bitmap^ bmp, bool frontalPrior, float scale, int min_nb, int min_size, int max_size)
{
	int stride;
	unsigned char* buf = toGray(bmp, stride);
	List<Rectangle> ^r;
	if (frontalPrior)
		r = toList(facedetect_frontal(buf, bmp->Width, bmp->Height, stride, scale, min_nb, min_size, max_size));
	if (r && r->Count == 0)
		List<Rectangle> ^r = toList(facedetect_multiview(buf, bmp->Width, bmp->Height, stride, scale, min_nb, min_size, max_size));
	delete[] buf;
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

// 0.30d = 0.0100110b
// 0.59d = 0.1001011b
// 0.11d = 0.0001110b
inline int s1mul0_3(int x)
{
	return (x >> 1) + (x >> 4) + (x >> 5);
}

inline int s1mul0_59(int x)
{
	return (x >> 0) + (x >> 3) + (x >> 5) + (x >> 6);
}

inline int s1mul0_11(int x)
{
	return (x >> 3) + (x >> 4) + (x >> 5);
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
	unsigned char* pBmp = (unsigned char*)bmpd->Scan0.ToPointer(),
		*b, *g, *r;
	for (int offset = 0, y = 0; y < bmp->Height; ++y, offset += bmpd->Stride){
		b = pBmp + offset + 0, g = pBmp + offset + 1, r = pBmp + offset + 2;
		for (int x = 0; x < bmpd->Width; ++x, b += 3, g += 3, r += 3) {
			res[y * stride + x] = (unsigned char)((float)*r * 0.3f + (float)*g * 0.59f + (float)*b * 0.11f);
			//res[y * stride + x] = (unsigned char)(((s1mul0_3((int)*r)) + (s1mul0_59((int)*g)) + (s1mul0_11((int)*b))) >> 1);
		}
	}
	bmp->UnlockBits(bmpd);
	return res;
}
