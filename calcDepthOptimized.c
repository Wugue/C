// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "calcDepthOptimized.h"
#include "utils.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */

/*float displacementOptimized(int dx, int dy)
{
	float x[4] = {dx, dy, 0, 0};
	__m128 t = _mm_load_ps(x);
	t = _mm_mul_ps(t,t);
	_mm_storeu_ps(x, t);
	float displacement = (float) sqrt(x[0]+x[1]);
	return displacement;
}*/


//  Current Speed: non-odd: 5.70546    odd: 4.4134
float displacementOptimized(int dx, int dy)
{
	float squaredDisplacement = dx * dx + dy * dy;
	float displacement = sqrt(squaredDisplacement);
	return displacement;
}
void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
    omp_set_num_threads(imageHeight/2);
		#pragma omp parallel for
	/* The two outer for loops iterate through each pixel */
		for (int y = 0; y < imageHeight; y++)
		{
//		    fprintf(stdout, "loopY\n");
			if ((y < featureHeight) || (y >= imageHeight - featureHeight)){
				__m128 c = _mm_setzero_ps();
				for (int i = y*imageWidth; i < (((imageWidth)/4 *4)+y*imageWidth); i+=4){
					_mm_storeu_ps( (__m128*)( i+depth), c);
				}
				for (int i = (((imageWidth)/4 *4)+y*imageWidth); i < (y+1)*imageWidth; i++){
					depth[i] = 0;
				}
				continue;
			}
			for (int x = 0; x < imageWidth; x++)
			{
			   
//			    fprintf(stdout, "loopX\n");
				/* Set the depth to 0 if looking at edge of the image where a feature box cannot fit. */
				if ((x < featureWidth) || (x >= imageWidth - featureWidth))
				{
					depth[y * imageWidth + x] = 0;
					continue;
				}
	
				float minimumSquaredDifference = -1;
				int minimumDy = 0;
				int minimumDx = 0;

				/* Iterate through all feature boxes that fit inside the maximum displacement box. 
				   centered around the current pixel. */


					for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy+=1)
					{
//						fprintf(stdout, "loop1\n");
						if ( y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight)
						{
							continue;
						}
						for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
						{
//						    fprintf(stdout, "loop2\n");
							/* Skip feature boxes that dont fit in the displacement box. */
							if (x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
							{
								continue;
							}
			
							float squaredDifference = 0;
						/*	for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
								{
								int rightY = y + dy + boxY;
								int leftY = y + boxY;
								for (int boxX = -featureWidth; boxX <= featureWidth; boxX++)
									{
									int leftX = x + boxX;
									int rightX = x + dx + boxX;

									float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
									squaredDifference += difference * difference;
								}
							} */
							__m128 a = _mm_setzero_ps();
							/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
							int rightY, leftY, leftX, rightX;
							float l1, l2, l3, r1, r2, r3, d1, d2, d3;
							__m128 b1 = _mm_setzero_ps();
							__m128 b2 = _mm_setzero_ps();
							__m128 b3 = _mm_setzero_ps();
							__m128 c1 = _mm_setzero_ps();
							__m128 c2 = _mm_setzero_ps();
							__m128 c3 = _mm_setzero_ps();
							/*
							for (int boxY = -featureHeight; boxY < (2*featureHeight)/3*3 - featureHeight; boxY+=3)
							{
								rightY = y + dy + boxY;
								leftY = y + boxY;
//								fprintf(stdout, "main case:\n");
								for (int boxX = -featureWidth; boxX < (2*featureWidth)/4*4 - featureWidth; boxX+=4)
								{
									
//									fprintf(stdout, "boxX = %d, boxY = %d\n", boxX, boxY); 
									leftX = x + boxX;
									rightX = x + dx + boxX;
									
									b1 = _mm_loadu_ps(&left[leftY * imageWidth + leftX]);
									b2 = _mm_loadu_ps(&left[(leftY+1) * imageWidth + leftX]);
									b3 = _mm_loadu_ps(&left[(leftY+2) * imageWidth + leftX]);
//									fprintf(stdout, "here\n");
									c1 = _mm_loadu_ps(&right[rightY * imageWidth + rightX]);
									c2 = _mm_loadu_ps(&right[(rightY+1) * imageWidth + rightX]);
									c3 = _mm_loadu_ps(&right[(rightY+2) * imageWidth + rightX]);
									
									b1 = _mm_sub_ps(b1,c1);
									b2 = _mm_sub_ps(b2,c2);
									b3 = _mm_sub_ps(b3,c3);
									
									b1 = _mm_mul_ps(b1,b1);
									b2 = _mm_mul_ps(b2,b2);
									b3 = _mm_mul_ps(b3,b3);
									
									a = _mm_add_ps(a,b1);
									a = _mm_add_ps(a,b2);
									a = _mm_add_ps(a,b3);
								}

								for (int boxX = (2*featureWidth)/4*4 - featureWidth; boxX <= featureWidth; boxX++)
								{
									
//									fprintf(stdout, "boxX = %d, boxY = %d\n", boxX, boxY); 
									leftX = x + boxX;
									rightX = x + dx + boxX;
									l1 = left[leftY * imageWidth + leftX];
									l2 = left[(leftY+1) * imageWidth + leftX];
									l3 = left[(leftY+2) * imageWidth + leftX];
									
									r1 = right[rightY * imageWidth + rightX];
									r2 = right[(rightY+1) * imageWidth + rightX];
									r3 = right[(rightY+2) * imageWidth + rightX];
									
									d1 = l1-r1;
									d2 = l2-r2;
									d3 = l3-r3;
									
									squaredDifference += d1 * d1;
									squaredDifference += d2 * d2;
									squaredDifference += d3 * d3;
								}
							}*/

							for (int boxY = -featureHeight; boxY < (2*featureHeight)/3*3 - featureHeight; boxY+=3)
							{
								rightY = y + dy + boxY;
								leftY = y + boxY;
//								fprintf(stdout, "main case:\n");
								for (int boxX = -featureWidth; boxX < (2*featureWidth)/4*4 - featureWidth; boxX+=4)
								{
									
//									fprintf(stdout, "boxX = %d, boxY = %d\n", boxX, boxY); 
									leftX = x + boxX;
									rightX = x + dx + boxX;
									
									b1 = _mm_loadu_ps(&left[leftY * imageWidth + leftX]);
									b2 = _mm_loadu_ps(&left[(leftY+1) * imageWidth + leftX]);
									b3 = _mm_loadu_ps(&left[(leftY+2) * imageWidth + leftX]);
//									fprintf(stdout, "here\n");
									c1 = _mm_loadu_ps(&right[rightY * imageWidth + rightX]);
									c2 = _mm_loadu_ps(&right[(rightY+1) * imageWidth + rightX]);
									c3 = _mm_loadu_ps(&right[(rightY+2) * imageWidth + rightX]);
									
									b1 = _mm_sub_ps(b1,c1);
									b2 = _mm_sub_ps(b2,c2);
									b3 = _mm_sub_ps(b3,c3);
									
									b1 = _mm_mul_ps(b1,b1);
									b2 = _mm_mul_ps(b2,b2);
									b3 = _mm_mul_ps(b3,b3);
									
									a = _mm_add_ps(a,b1);
									a = _mm_add_ps(a,b2);
									a = _mm_add_ps(a,b3);
								}

								
							}

							for (int boxY = -featureHeight; boxY < (2*featureHeight)/3*3 - featureHeight; boxY+=3)
							{
								rightY = y + dy + boxY;
								leftY = y + boxY;
//								fprintf(stdout, "main case:\n");
								

								for (int boxX = (2*featureWidth)/4*4 - featureWidth; boxX <= featureWidth; boxX++)
								{
									
//									fprintf(stdout, "boxX = %d, boxY = %d\n", boxX, boxY); 
									leftX = x + boxX;
									rightX = x + dx + boxX;
									l1 = left[leftY * imageWidth + leftX];
									l2 = left[(leftY+1) * imageWidth + leftX];
									l3 = left[(leftY+2) * imageWidth + leftX];
									
									r1 = right[rightY * imageWidth + rightX];
									r2 = right[(rightY+1) * imageWidth + rightX];
									r3 = right[(rightY+2) * imageWidth + rightX];
									
									d1 = l1-r1;
									d2 = l2-r2;
									d3 = l3-r3;
									
									squaredDifference += d1 * d1;
									squaredDifference += d2 * d2;
									squaredDifference += d3 * d3;
								}
							}
							
//							fprintf(stdout, "edge case/n");
							for (int boxY = (2*featureHeight)/3*3 - featureHeight; boxY <= featureHeight; boxY++)
							{
								rightY = y + dy + boxY;
								leftY = y + boxY;
								for (int boxX = -featureWidth; boxX < (2*featureWidth)/4*4 - featureWidth; boxX+=4)
								{
									
//									fprintf(stdout, "boxX = %d, boxY = %d \n" , boxX, boxY); 
									leftX = x + boxX;
									rightX = x + dx + boxX;
									
									b1 = _mm_loadu_ps(&left[leftY * imageWidth + leftX]);
//									fprintf(stdout, "here\n");
									c1 = _mm_loadu_ps(&right[rightY * imageWidth + rightX]);
									
									b1 = _mm_sub_ps(b1,c1);
									
									b1 = _mm_mul_ps(b1,b1);
									
									a = _mm_add_ps(a,b1);
								}
								for (int boxX = (2*featureWidth)/4*4 - featureWidth; boxX <= featureWidth; boxX++)
								{
									
//									fprintf(stdout, "boxX = %d, boxY = %d \n", boxX, boxY); 
									leftX = x + boxX;
									rightX = x + dx + boxX;
									d1 = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
									
									squaredDifference += d1 * d1;
								}
							}
							float x[4];
//							fprintf(stdout, "loop4.4\n");
							_mm_storeu_ps(x, a);
									
							for(int i =0; i<4; i++){
//								fprintf(stdout, "loop4.5\n");
								squaredDifference +=x[i];
							}
							/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */	
							/* 
							Check if you need to update minimum square difference. 
							This is when either it has not been set yet, the current
							squared displacement is equal to the min and but the new
							displacement is less, or the current squared difference
							is less than the min square difference.
							*/
							int c = (minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && (displacementOptimized(dx, dy) < displacementOptimized(minimumDx, minimumDy))) || (minimumSquaredDifference > squaredDifference);
							minimumSquaredDifference = c ? squaredDifference: minimumSquaredDifference;
							minimumDx  = c ? dx: minimumDx;
							minimumDy  = c ? dy: minimumDy;
						}
					}	
					/* 
					Set the value in the depth map. 
					If max displacement is equal to 0, the depth value is just 0.
					*/
					depth[y * imageWidth + x] = ((minimumSquaredDifference != -1) && (maximumDisplacement !=0)) ? displacementOptimized(minimumDx, minimumDy) : 0;
/*					if (minimumSquaredDifference != -1)
					{
						if (maximumDisplacement == 0)
						{
							depth[y * imageWidth + x] = 0;
						}
						else	
						{
							depth[y * imageWidth + x] = displacementOptimized(minimumDx, minimumDy);
						}
					}
					else
					{
						depth[y * imageWidth + x] = 0;
					} */
				}
		}
}
