//
// YARPBlobFinder.cpp
//

#include "YARPBlobFinder.h"

#ifndef MaxBoxesBlobFinder
#define MaxBoxesBlobFinder 65000
#endif

#ifndef MaxBoxes
#define MaxBoxes 255
#endif

#ifndef MaxTags
#define MaxTags 65000
#endif

void YARPBlobFinder::MergeRegions(YARPImageOf<YarpPixelInt>& src, int rTag, int cTag)
{
	// for now, no reuse of tags
	// tag2 can be reused
	// tag1<tag2

	short tag1 = src(cTag, rTag-1);
	short tag2 = src(cTag-1, rTag);

	
	// in realtà deve scandire tutte le righe
	// tranne la corrente che deve fare fino a cTag
	for (int r=42; r<=rTag; r++) {
		for (int c=0; c<width; c++) {
			if(src(c, r) == tag2)
				src(c, r) = tag1;
		}
	}
	src(cTag-1, rTag) = tag1;
	src(cTag, rTag) = tag1;
	
	// partial reuse
	if (last_tag==tag2) last_tag--;
}


// uses short instead of a std image.
int YARPBlobFinder::SpecialTagging(YARPImageOf<YarpPixelInt>& dst, YARPImageOf<YarpPixelMono>& img)
{
	const int th=12;

	IplImage *source = img.GetIplPointer();
	IplImage *tagmap = dst.GetIplPointer();

	unsigned char *src = (unsigned char *)source->imageData;
	unsigned char *tagged = (unsigned char *)tagmap->imageData;

	last_tag = 1;
	int left_tag, up_tag;

	for (int i=0; i<MaxTags+1; i++)
		arrayTags1[i]=i;
	
	memset(arrayTags2, 0, sizeof(long int)*(MaxTags+1));

	last_tag++;
	num_tag++;

	// first row
	int r=0;
	for (int c=0; c<width; c++)
		dst(c, r) = last_tag;
	
	//for (r=42; r<height; r++) {
	for (r=1; r<height; r++) {
		int c=0;

		unsigned char val = img(c, r);

		//if (val != 0) {

			//if (r > 42) {
			//if (r > 0) {
				// with this condition I set the max change within a pixel: the max derivative
				if ( abs(val-img(c, r-1))<th )
					up_tag = dst(c, r-1);
				else
					up_tag = 0;
			//} else
			//	up_tag = 0;

			if (up_tag) 
				// inherit from the top 
				dst(c, r) = up_tag;
			else {
				// gets a new tag 
				last_tag++;
				num_tag++;
				if (last_tag <= MaxTags)
					dst(c, r) = last_tag;
				else
					// everything under tag MaxTags (?).
					dst(c, r) = MaxTags;
			}	
	}

	//for (r=42; r<height; r++) {
	for (r=1; r<height; r++) {
		for (int c=1; c<width; c++)	{

			unsigned char val = img(c, r);

			//if (val != 0) {
				if ( abs(val-img(c-1, r))<th )
					left_tag = dst(c-1, r);
				else
					left_tag = 0;

				//if (r > 42) {
				//if (r > 1) {
					if ( abs(val-img(c, r-1))<th )
						up_tag = dst(c, r-1);
					else
						up_tag = 0;
				//} else
				//	up_tag = 0;

				if (left_tag) {
					if (up_tag) {
						// both left and up tagged, may need to merge 
						if(arrayTags1[left_tag] != arrayTags1[up_tag]) {
							num_tag--;
							
							/*int tmp=arrayTags1[left_tag];
							
							listsTags[arrayTags1[up_tag]].Push(tmp);
							
							arrayTags1[left_tag]=arrayTags1[up_tag];
							
							while (!listsTags[tmp].IsEmpty()) {
								int val=listsTags[tmp].Pop();
								arrayTags1[val]=arrayTags1[up_tag];
								listsTags[arrayTags1[up_tag]].Push(val);
							}*/

							//int conta=0;

							int tmp=arrayTags1[left_tag];
							
							for (int j=0; j<=last_tag; j++) {
								if (arrayTags1[j]==tmp) {
									arrayTags1[j]=arrayTags1[up_tag];
									//conta++;
								}
							}
							
							//ACE_ASSERT(conta == arrayTags2[tmp]+1);
							
							//arrayTags2[arrayTags1[up_tag]]+=(arrayTags2[tmp]+1);
							//arrayTags2[tmp]=0;
							
							dst(c,r) = up_tag;
	
							// partial reuse
							/*if (last_tag==left_tag) {
								last_tag--;
							}*/
						
						} else
							dst(c, r) = left_tag;
					} else 
						// inherit from the left 
						dst(c, r) = left_tag;
				} else {
					if (up_tag) 
						// inherit from the top 
						dst(c, r) = up_tag;
					else {
						// gets a new tag 
						last_tag++;
						num_tag++;
						if (last_tag <= MaxTags)
							dst(c, r) = last_tag;
						else
							// everything under tag MaxTags (?).
							dst(c, r) = MaxTags;
					}	
				}
			//} else
			//	dst(c, r) = 0;
		}
	}

	for (r=1; r<height; r++) {
		unsigned char val = img(0, r);

		if ( abs(val-img(width-1, r))<th ) {
			left_tag = dst(width-1, r);
			if (left_tag!=dst(0, r)) {
				num_tag--;
				
				int tmp=arrayTags1[left_tag];
				
				for (int j=0; j<=last_tag; j++) {
					if (arrayTags1[j]==tmp) {
						arrayTags1[j]=arrayTags1[dst(0, r)];
						//conta++;
					}
				}
			}
		}
	}

	return last_tag;
}


void YARPBlobFinder::RemoveUnsedTags(YARPImageOf<YarpPixelInt>& tags)
{
	bool *used = new bool [last_tag];
	unsigned int *newtags = new unsigned int [last_tag];

	for (int r=42; r<height; r++)
		for (int c=0; c<width; c++)
			used[tags(c, r)]=true;

	int j=0;
	for (int i=0; i<last_tag; i++)
		if (used[i]) {
			newtags[i]=j;
			j++;
		}

	for (r=42; r<height; r++)
		for (int c=0; c<width; c++)
			tags(c, r)=newtags[tags(c, r)];

	delete [] used;
	delete [] newtags;
}


// devo chiamare quello base + resize
YARPBlobFinder::YARPBlobFinder(int x, int y) 
{
	last_tag = 0;
	num_tag = 0;

	m_boxes = new YARPBox[MaxBoxesBlobFinder]; 
	ACE_ASSERT(m_boxes != NULL);

	m_attn = new YARPBox[MaxTags];
	ACE_ASSERT(m_attn != NULL);

	// zero is not used
	arrayTags1 = new long int [MaxTags+1];
	ACE_ASSERT(arrayTags1 != NULL);

	// zero is not used
	arrayTags2 = new long int [MaxTags+1];
	ACE_ASSERT(arrayTags2 != NULL);

	pixelSize = new double [m_lp.GetCHeight()];
	for (int r=0; r<m_lp.GetCHeight(); r++) {
		pixelSize[r]=m_lp.GetPixelSize(r);
		//pixelSize[r]=m_lp.Jacobian(r, 0);
	}
	
	for (int i=0; i<MaxTags+1; i++)
		arrayTags1[i]=i;
	
	//listsTags = new YARPLinkedList [MaxTags+1];
	//ACE_ASSERT(listsTags != NULL);
	
	//m_tagged = new short[x * y];

	// defaults.
	//m_sat_thr = 25;	// 0.1 is the best resolution in saturation
	//m_hue_thr = 10; // 36 degrees is the resolution in hue

	width=x;
	height=y;
}


YARPBlobFinder::YARPBlobFinder() 
{
	using namespace _logpolarParams;
	
	last_tag = 0;
	num_tag = 0;
	m_boxes = new YARPBox[MaxBoxesBlobFinder]; 
	ACE_ASSERT(m_boxes != NULL);

	m_attn = new YARPBox[MaxTags];
	ACE_ASSERT(m_attn != NULL);

	// zero is not used
	arrayTags1 = new long int [MaxTags+1];
	ACE_ASSERT(arrayTags1 != NULL);

	// zero is not used
	arrayTags2 = new long int [MaxTags+1];
	ACE_ASSERT(arrayTags2 != NULL);

	//double area=0.0;
	//double area2=0.0;
	pixelSize = new double [_srho];
	for (int r=0; r<_srho; r++) {
		pixelSize[r]=m_lp.GetPixelSize(r);
		//area+=pixelSize[r];
		//area2+=m_lp.Jacobian(r, 0);
	}
	
	for (int i=0; i<MaxTags+1; i++)
		arrayTags1[i]=i;

	//listsTags = new YARPLinkedList [MaxTags+1];
	//ACE_ASSERT(listsTags != NULL);
}


void YARPBlobFinder::Resize(int x, int y)
{
	width=x;
	height=y;
}


void YARPBlobFinder::Cleanup()
{
	if (m_boxes != NULL)
		delete[] m_boxes;
	m_boxes = NULL;

	if (m_attn != NULL)
		delete[] m_attn;
	m_attn = NULL;

	if (arrayTags1 != NULL)
		delete[] arrayTags1;
	arrayTags1 = NULL;

	if (arrayTags2 != NULL)
		delete[] arrayTags2;
	arrayTags2 = NULL;
}

/*void YARPBlobFinder::Apply(const YARPImageOf<YarpPixelBGR>& is, YARPImageOf<YarpPixelBGR>& id)
{
	ACE_ASSERT(id.GetHeight() == is.GetHeight() && id.GetWidth() == is.GetWidth());

	_apply(is, id);
}

void YARPBlobFinder::Apply(const YARPImageOf<YarpPixelRGB>& is, YARPImageOf<YarpPixelRGB>& id)
{
	ACE_ASSERT(id.GetHeight() == is.GetHeight() && id.GetWidth() == is.GetWidth());

	_apply (is, id);
}*/

void YARPBlobFinder::Apply(YARPImageOf<YarpPixelMono>& is, YARPImageOf<YarpPixelInt>& id, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r, YARPImageOf<YarpPixelMono> &g, YARPImageOf<YarpPixelMono> &b, bool remove)
{
	ACE_ASSERT(id.GetHeight() == is.GetHeight() && id.GetWidth() == is.GetWidth());

	SpecialTagging(id, is);

	blobCatalog(id, is, rg, gr, by, r, g, b, last_tag);
	if (remove) RemoveNonValid();
	//MergeBoxes(); //further clustering not needed.

	// merge boxes which are too close.
	// statically. Clearly this procedure could be more effective 
	// if it takes time into account.
	// LATER: update also the logpolar coordinates during
	//	the merger of the boxes.
}

void YARPBlobFinder::blobCatalog(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono>& img, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r1, YARPImageOf<YarpPixelMono> &g1, YARPImageOf<YarpPixelMono> &b1, int lasttag)
{
	//SpecialTagging(tagged, img);

	// I've got the tagged image now.
	ACE_ASSERT(last_tag <= MaxBoxesBlobFinder);

	last_tag=lasttag;

	//printf("last_tag is %d\n", last_tag);

	for(int i = 0; i <= last_tag; i++) {
		m_boxes[i].cmax = m_boxes[i].rmax = 0;
		m_boxes[i].cmin = width;
		m_boxes[i].rmin = height;

		m_boxes[i].xmax = m_boxes[i].ymax = 0;
		m_boxes[i].xmin = m_lp.GetCWidth();
		m_boxes[i].ymin = m_lp.GetCHeight();

		//m_boxes[i].total_sal = 0;
		m_boxes[i].areaLP = 0;
		//m_boxes[i].areaCart = 0;
		m_boxes[i].xsum = m_boxes[i].ysum = 0;
		m_boxes[i].valid = false;
		
		m_boxes[i].id = i;
		
		//m_boxes[i].edge = false;

		m_boxes[i].rgSum=0;
		m_boxes[i].grSum=0;
		m_boxes[i].bySum=0;

		m_boxes[i].rSum=0;
		m_boxes[i].gSum=0;
		m_boxes[i].bSum=0;

	}
  
	// special case for the null tag (0)
	// null tag is not used!!!
	m_boxes[0].rmax = m_boxes[0].rmin = height/2;
	m_boxes[0].cmax = m_boxes[0].cmin = width/2;
	m_boxes[0].xmax = m_boxes[0].xmin = m_lp.GetCWidth() / 2;
	m_boxes[0].ymax = m_boxes[0].ymin = m_lp.GetCHeight() / 2;
	m_boxes[0].valid = true;

	// pixels are logpolar, averaging is done in cartesian.
	//unsigned char *source = (unsigned char *)img.GetArray();
	//short *tmp = tagged;
	for(int r = 0; r < height; r++)
		for(int c = 0; c < width; c++) {
			long int tag_index = arrayTags1[tagged(c, r)];
			if (tag_index != 0) {
				m_boxes[tag_index].areaLP++;
				//m_boxes[tag_index].areaCart+=pixelSize[r];

				m_boxes[tag_index].valid = true;

				// euristic to check if the blob is an edge
				//if (img(c,r)>70) m_boxes[tag_index].edge = true;
				
				int x, y;
				m_lp.Logpolar2Cartesian(r, c, x, y);

				if (m_boxes[tag_index].ymax < y) m_boxes[tag_index].ymax = y;
				if (m_boxes[tag_index].ymin > y) m_boxes[tag_index].ymin = y;
				if (m_boxes[tag_index].xmax < x) m_boxes[tag_index].xmax = x;
				if (m_boxes[tag_index].xmin > x) m_boxes[tag_index].xmin = x;

				/*m_boxes[tag_index].ysum += y;
				m_boxes[tag_index].xsum += x;*/

				if (m_boxes[tag_index].rmax < r) m_boxes[tag_index].rmax = r;
				if (m_boxes[tag_index].rmin > r) m_boxes[tag_index].rmin = r;
				if (m_boxes[tag_index].cmax < c) m_boxes[tag_index].cmax = c;
				if (m_boxes[tag_index].cmin > c) m_boxes[tag_index].cmin = c;

				m_boxes[tag_index].rgSum += rg(c, r);
				m_boxes[tag_index].grSum += gr(c, r);
				m_boxes[tag_index].bySum += by(c, r);

				m_boxes[tag_index].rSum += r1(c, r);
				m_boxes[tag_index].gSum += g1(c, r);
				m_boxes[tag_index].bSum += b1(c, r);
			}
		}

	// The blob in the fovea is marked as non valid
	// so it doesn't appear in the attentional map
	m_boxes[arrayTags1[tagged(0, 0)]].valid=false;
}	


void YARPBlobFinder::ComputeSalience()
{	
	//double max_areaCart;
	int max_areaCart;
	int i;

	bool *taken = new bool[last_tag];

	memset(taken, false, sizeof(bool)*last_tag);
	
	/*for (int i=0; i < last_tag; i++)
		taken[i]=false;*/

	// create now the subset of attentional boxes.
	for (int box_num = 0; box_num < MaxBoxes; box_num++) {
		// find the most frequent tag, zero does not count 
		max_tag = 0;
		max_areaCart = 0;
		for(i = 1; i < last_tag; i++) {
			int area = TotalArea(m_boxes[i]);
			if (area > max_areaCart && !taken[i] && m_boxes[i].valid)
			//if(m_boxes[i].areaCart > max_areaCart)
			{
				max_areaCart= area;
				//max_areaCart= m_boxes[i].areaCart;
				max_tag = i;
			}
		}
	
		if (max_tag != 0) {
			m_attn[box_num] = m_boxes[max_tag];

			//m_attn[box_num].valid = true;
			//m_attn[box_num].centroid_y = m_boxes[max_tag].ysum / max_areaCart;
			//m_attn[box_num].centroid_x = m_boxes[max_tag].xsum / max_areaCart;

			// mean values are calculated only for biggest blobs
			m_attn[box_num].meanRG = m_boxes[max_tag].rgSum / m_boxes[max_tag].areaLP;
			m_attn[box_num].meanGR = m_boxes[max_tag].grSum / m_boxes[max_tag].areaLP;
			m_attn[box_num].meanBY = m_boxes[max_tag].bySum / m_boxes[max_tag].areaLP;
			
			//m_attn[box_num].id = max_tag;
			
			taken[max_tag]=true;
		} else {
			// return the center, no motion
			m_attn[box_num].valid = false;
			//m_attn[box_num].centroid_x = m_lp.GetCWidth() / 2;
			//m_attn[box_num].centroid_y = m_lp.GetCHeight() / 2;
		}
	}

	//PrintBoxes (m_attn, MaxBoxes);

	// I've here MaxBoxes boxes accounting for the largest 
	// regions in the image.
	// remember that there's a bias because of logpolar.

	delete [] taken;
}


void YARPBlobFinder::ComputeMeanColors()
{	
	// create the subset of attentional boxes.
	for (int i = 1; i < last_tag; i++)
		if (m_boxes[i].areaLP!=0) {
			m_boxes[i].meanColors.r = m_boxes[i].rSum / m_boxes[i].areaLP;
			m_boxes[i].meanColors.g = m_boxes[i].gSum / m_boxes[i].areaLP;
			m_boxes[i].meanColors.b = m_boxes[i].bSum / m_boxes[i].areaLP;
		}
}


/*void YARPBlobFinder::PrintBoxes(YARPBox *m_boxes, int size)
{
	for (int i = 0; i < size; i++) {
		printf ("box : %d\n", i);

		if (m_boxes[i].valid) {
			printf ("area : %d\n", TotalArea (m_boxes[i]));

			printf ("cmin, cmax : %d %d\n", m_boxes[i].cmin, m_boxes[i].cmax);
			printf ("rmin, rmax : %d %d\n", m_boxes[i].rmin, m_boxes[i].rmax);
			printf ("xmin, xmax : %d %d\n", m_boxes[i].xmin, m_boxes[i].xmax);
			printf ("ymin, ymax : %d %d\n", m_boxes[i].ymin, m_boxes[i].ymax);
			printf ("total_sal : %d\n", m_boxes[i].total_sal);
			printf ("total_pix : %d\n", m_boxes[i].areaLP);
			printf ("sx, sy : %d %d\n", m_boxes[i].xsum, m_boxes[i].ysum);
		}
	}

}*/


void YARPBlobFinder::RemoveNonValid()
{
	const int max_size = m_lp.GetCWidth() * m_lp.GetCHeight() / 13; // ~area/10
	const int min_size = 100;

	for (int i = 1; i < last_tag;i++) {
		int area = TotalArea(m_boxes[i]);
		if (// in effetti eliminare i blob troppo piccoli nn è molto utile visto
			// ke i blob vengono comunque ordinati x grandezza
			//area < min_size ||
			area > max_size )
			//m_boxes[i].edge==true)
		{
			m_boxes[i].valid=false;
		}
	}

	/*for (int i = 1; i < last_tag;) {
		int area = TotalArea(m_boxes[i]);
		//if (m_boxes[i].valid == false || 
		//	m_boxes[i].areaLP < 5 ||
		//	area < min_size ||
		//	area > max_size)
		// If I delete the boxes with .areaLP > 255 than sum of color can be an int?
		//if (m_boxes[i].valid == false || 
		//	m_boxes[i].areaCart < min_size ||
		//	m_boxes[i].areaCart > max_size ||
		//	m_boxes[i].edge==true)
		if (m_boxes[i].valid == false || 
			area < min_size ||
			area > max_size ||
			m_boxes[i].edge==true)
		{
			if (i == (last_tag-1))
				last_tag--;
			else {
				memcpy(&(m_boxes[i]), &(m_boxes[i+1]), sizeof(YARPBox) * (last_tag - i - 1));
				last_tag--;
			}
		} else
			i++;
	}*/
}


/*void YARPBlobFinder::MergeBoxes (void)
{
	// do not take i = 0 into account.
	//const double overlap = 1.5;
	for (int i = 1; i <= last_tag-1; i++) {
		int j = i + 1;
		while (j <= last_tag) {
			//if (CenterDistance (i, j) < CombinedSize (i, j) * overlap &&
			if (Overlap (i, j) && HueDistance (i, j) < m_hue_thr/2 ) {
				FuseBoxes (i, j);
				if (j == last_tag) {
					last_tag--;
				} else {
					memcpy (&(m_boxes[j]), &(m_boxes[j+1]), sizeof(YARPBox) * (last_tag - j ));
					last_tag--;
				}
			} else
				j++;
		}
	}
}*/

// draw a white box.
void YARPBlobFinder::DrawBox(IplImage *img, int x1, int y1, int x2, int y2)
{
	ACE_ASSERT(x1 <= x2 && y1 <= y2);
	ACE_ASSERT(img->nChannels == 1);
	const int w = img->widthStep;
	const int h = img->height;
	unsigned char *dst = (unsigned char *)img->imageData;

	unsigned char *ptr1 = dst + y1 * w + x1;
	unsigned char *ptr2 = dst + y2 * w + x1;
	for (int i = 0; i <= x2-x1; i++) {
		*ptr1++ = 255;
		*ptr2++ = 255;
	}

	ptr1 = dst + y1 * w + x1;
	ptr2 = dst + y1 * w + x2;
	for (i = 0; i <= y2-y1; i++) {
		*ptr1 = 255;
		*ptr2 = 255;
		ptr1 += w;
		ptr2 += w;
	}
}

// draw a colored box.
void YARPBlobFinder::DrawBox(IplImage *img, int x1, int y1, int x2, int y2, int r, int g, int b)
{
	ACE_ASSERT(x1 <= x2 && y1 <= y2);
	ACE_ASSERT(img->nChannels == 3);

	const int w = img->width;
	const int h = img->height;
	unsigned char *dst = (unsigned char *)img->imageData;

	unsigned char *ptr1 = dst + y1 * w * 3 + x1 * 3;
	unsigned char *ptr2 = dst + y2 * w * 3 + x1 * 3;
	for (int i = 0; i <= x2-x1; i++) {
		*ptr1++ = r;
		*ptr1++ = g;
		*ptr1++ = b;
		*ptr2++ = r;
		*ptr2++ = g;
		*ptr2++ = b;
	}

	ptr1 = dst + y1 * w * 3 + x1 * 3;
	ptr2 = dst + y1 * w * 3 + x2 * 3;
	for (i = 0; i <= y2-y1; i++) {
		*ptr1++ = r;
		*ptr1++ = g;
		*ptr1++ = b;
		ptr1 += (w * 3 - 3);
		*ptr2++ = r;
		*ptr2++ = g;
		*ptr2++ = b;
		ptr2 += (w * 3 - 3);
	}
}

void YARPBlobFinder::DrawBoxes(YARPImageOf<YarpPixelMono>& id)
{
	for (int i = 0; i < MaxBoxes; i++) {
		if (m_attn[i].valid) {
			DrawBox(id.GetIplPointer(), 
				m_attn[i].xmin,
				m_attn[i].ymin,
				m_attn[i].xmax,
				m_attn[i].ymax);
		}
	}
}

void YARPBlobFinder::DrawBoxesLP(YARPImageOf<YarpPixelMono>& id)
{
	for (int i = 0; i < MaxBoxes; i++) {
		if (m_attn[i].valid) {
			DrawBox(id.GetIplPointer(), 
				m_attn[i].cmin,
				m_attn[i].rmin,
				m_attn[i].cmax,
				m_attn[i].rmax);
		}
	}
}

void YARPBlobFinder::DrawBoxes(YARPImageOf<YarpPixelRGB>& id)
{
	for (int i = 0; i < MaxBoxes; i++) {
		if (m_attn[i].valid) {
			DrawBox(id.GetIplPointer(), 
				m_attn[i].xmin,
				m_attn[i].ymin,
				m_attn[i].xmax,
				m_attn[i].ymax,
				255, 0, 0);		// Use red.
		}
	}
}

void YARPBlobFinder::DrawBoxes(YARPImageOf<YarpPixelBGR>& id)
{
	for (int i = 0; i < MaxBoxes; i++) {
		if (m_attn[i].valid) {
			DrawBox(id.GetIplPointer(), 
				m_attn[i].xmin,
				m_attn[i].ymin,
				m_attn[i].xmax,
				m_attn[i].ymax,
				0, 0, 255);		// Use red.
		}
	}
}


int YARPBlobFinder::DrawMeanColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged)
{
	/*if (last_tag<numBlob || numBlob==-1) numBlob=last_tag;
	
	for (int i = 0; i < numBlob; i++) {
	//for (int i = 0; i < MaxBoxes; i++) {
		for (int r=m_boxes[i].rmin; r<=m_boxes[i].rmax; r++)
			for (int c=m_boxes[i].cmin; c<=m_boxes[i].cmax; c++)
				if (arrayTags1[tagged(c, r)]==m_boxes[i].id) {
					id(c ,r)=m_boxes[i].meanColors;
				}
	}

	return numBlob;*/

	for (int r=0; r<height; r++)
		for (int c=0; c<width; c++)
			id(c ,r)=m_boxes[arrayTags1[tagged(c, r)]].meanColors;
	
	return 1;
}


int YARPBlobFinder::DrawContrastLP(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float prg, float pgr, float pby)
{
	IplROI zdi;
	int salienceBU, salienceTD;
	int borderRG;
	int borderGR;
	int borderBY;

	int a1,b1,a2,b2;

	int minSalienceBU=INT_MAX;
	int maxSalienceBU=INT_MIN;

	int minSalienceTD=INT_MAX;
	int maxSalienceTD=INT_MIN;

	YARPImageOf<YarpPixelMono> tmp;

	tmp.Resize(width, height);
	tmp.Zero();
	
	zdi.coi=0;
	zdi.width=1;
	zdi.height=1;

	//((IplImage *)tmp)->roi=&zdi;

	borderRG=((IplImage *)rg)->BorderMode[IPL_SIDE_BOTTOM_INDEX];
	iplSetBorderMode((IplImage *)rg, IPL_BORDER_REPLICATE, IPL_SIDE_BOTTOM, 0);
	borderGR=((IplImage *)gr)->BorderMode[IPL_SIDE_BOTTOM_INDEX];
	iplSetBorderMode((IplImage *)gr, IPL_BORDER_REPLICATE, IPL_SIDE_BOTTOM, 0);
	borderBY=((IplImage *)by)->BorderMode[IPL_SIDE_BOTTOM_INDEX];
	iplSetBorderMode((IplImage *)by, IPL_BORDER_REPLICATE, IPL_SIDE_BOTTOM, 0);

	// ARRONZAMENTO
	//memset(dst.GetRawBuffer(), 255, ((IplImage*)dst)->imageSize);
	dst.Zero();
	
	if (MaxBoxes<numBlob) numBlob=MaxBoxes;
	if (max_tag<numBlob) numBlob=MaxBoxes;
	
	for (int i = 0; i < numBlob; i++) {
		if (m_attn[i].valid) {
			int rdim=(m_attn[i].rmax-m_attn[i].rmin+1);
			int cdim=(m_attn[i].cmax-m_attn[i].cmin+1);

			//zdi.xOffset=m_attn[i].cmin;
			//zdi.yOffset=m_attn[i].rmin;
			iplBlur((IplImage*) rg, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_attn[i].cRG=abs(tmp(m_attn[i].cmin, m_attn[i].rmin)-m_attn[i].meanRG);
			//m_attn[i].cRG=min(tmp(m_attn[i].cmin, m_attn[i].rmin), m_attn[i].meanRG)*prg;

			iplBlur((IplImage*) gr, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_attn[i].cGR=abs(tmp(m_attn[i].cmin, m_attn[i].rmin)-m_attn[i].meanGR);
			//m_attn[i].cGR=min(tmp(m_attn[i].cmin, m_attn[i].rmin), m_attn[i].meanGR)*pgr;
			
			iplBlur((IplImage*) by, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_attn[i].cBY=abs(tmp(m_attn[i].cmin, m_attn[i].rmin)-m_attn[i].meanBY);
			//m_attn[i].cBY=min(tmp(m_attn[i].cmin, m_attn[i].rmin), m_attn[i].meanBY)*pby;

			//usando il max nn posso usare coefficienti negativi!
			// con il max se un blob è rosso e verde nn va molto bene...
			/*salience=m_attn[i].cRG;

			if (salience<m_attn[i].cGR)
				salience=m_attn[i].cGR;

			if (salience<m_attn[i].cBY)
				salience=m_attn[i].cBY;*/

			//salience=255-(m_attn[i].cRG+m_attn[i].cGR+m_attn[i].cBY)/3;
			salienceBU=m_attn[i].cRG+m_attn[i].cGR+m_attn[i].cBY;

			salienceTD=m_attn[i].meanRG*prg+m_attn[i].meanGR*pgr+m_attn[i].meanBY*pby;

			//salience/=2; //serve???

			if (salienceTD<0) salienceTD=0;
			
			m_attn[i].salienceBU=salienceBU;
			m_attn[i].salienceTD=salienceTD;

			if (salienceBU<minSalienceBU)
				minSalienceBU=salienceBU;
			else if (salienceBU>maxSalienceBU)
				maxSalienceBU=salienceBU;

			if (salienceTD<minSalienceTD)
				minSalienceTD=salienceTD;
			else if (salienceTD>maxSalienceTD)
				maxSalienceTD=salienceTD;
		}
	}

	if (maxSalienceBU!=minSalienceBU) {
		a1=255*254/(maxSalienceBU-minSalienceBU);
		b1=1-a1*minSalienceBU/255;
	} else {
		a1=0;
		b1=0;
	}

	if (maxSalienceTD!=minSalienceTD) {
		a2=255*254/(maxSalienceTD-minSalienceTD);
		b2=1-a2*minSalienceTD/255;
	} else {
		a2=0;
		b2=0;
	}

	for (i = 0; i < numBlob; i++) {
		if (m_attn[i].valid) {
			for (int r=m_attn[i].rmin; r<=m_attn[i].rmax; r++)
				for (int c=m_attn[i].cmin; c<=m_attn[i].cmax; c++)
					if (arrayTags1[tagged(c, r)]==m_attn[i].id)
						dst(c ,r)=(a1*m_attn[i].salienceBU/255+b1+a2*m_attn[i].salienceTD/255+b2)/2;
		}
	}
	
	((IplImage *)rg)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=borderRG;
	((IplImage *)gr)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=borderGR;
	((IplImage *)by)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=borderBY;

	return numBlob;
}


int YARPBlobFinder::DrawGrayLP(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int numBlob)
{
	if (MaxBoxes<numBlob) numBlob=MaxBoxes;
	
	for (int i = 0; i < numBlob; i++) {
	//for (int i = 0; i < MaxBoxes; i++) {
		if (m_attn[i].valid) {
			for (int r=m_attn[i].rmin; r<=m_attn[i].rmax; r++)
				for (int c=m_attn[i].cmin; c<=m_attn[i].cmax; c++)
					if (arrayTags1[tagged(c, r)]==m_attn[i].id)
						id(c ,r)=MaxBoxes-i;
		}
	}

	return numBlob;
}


void YARPBlobFinder::DrawFoveaBlob(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged)
{
	long int tag=arrayTags1[tagged(0, 0)];
	
	for (int r=m_boxes[tag].rmin; r<=m_boxes[tag].rmax; r++)
		for (int c=m_boxes[tag].cmin; c<=m_boxes[tag].cmax; c++)
			if (arrayTags1[tagged(c, r)]==tag)
				id(c ,r)=255;
}


// should be inlined.
double YARPBlobFinder::CenterDistance(int i, int j)
{
	const double xi = double(m_boxes[i].xsum) / double(m_boxes[i].areaLP);
	const double yi = double(m_boxes[i].ysum) / double(m_boxes[i].areaLP);
	const double xj = double(m_boxes[j].xsum) / double(m_boxes[j].areaLP);
	const double yj = double(m_boxes[j].ysum) / double(m_boxes[j].areaLP);

	const double dx = xi - xj;
	const double dy = yi - yj;

	return dx * dx + dy * dy;
}


double YARPBlobFinder::CombinedSize(int i, int j)
{
	const double sx_i = double(m_boxes[i].xmax - m_boxes[i].xmin);
	const double sy_i = double(m_boxes[i].ymax - m_boxes[i].ymin);
	const double sx_j = double(m_boxes[j].xmax - m_boxes[j].xmin);
	const double sy_j = double(m_boxes[j].ymax - m_boxes[j].ymin);

	const double size_i = (sx_i > sy_i) ? sx_i : sy_i;
	const double size_j = (sx_j > sy_j) ? sx_j : sy_j;

	return (size_i + size_j) * (size_i + size_j) / 4;
}


void YARPBlobFinder::FuseBoxes(int i, int j)
{
	const int max_x = (m_boxes[i].xmax > m_boxes[j].xmax) ? m_boxes[i].xmax : m_boxes[j].xmax;
	const int min_x = (m_boxes[i].xmin < m_boxes[j].xmin) ? m_boxes[i].xmin : m_boxes[j].xmin;
	const int max_y = (m_boxes[i].ymax > m_boxes[j].ymax) ? m_boxes[i].ymax : m_boxes[j].ymax;
	const int min_y = (m_boxes[i].ymin < m_boxes[j].ymin) ? m_boxes[i].ymin : m_boxes[j].ymin;

	m_boxes[i].xmax = max_x;
	m_boxes[i].xmin = min_x;
	m_boxes[i].ymax = max_y;
	m_boxes[i].ymin = min_y;

	m_boxes[i].xsum += m_boxes[j].xsum;
	m_boxes[i].ysum += m_boxes[j].ysum;
	m_boxes[i].areaLP += m_boxes[j].areaLP;

	m_boxes[j].valid = false;
}


void YARPBlobFinder::SeedColor(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int x, int y, int col)
{
	long int seed=arrayTags1[tagged(x,y)];

	//if (m_boxes[seed].valid && m_boxes[seed].areaLP > 5 && m_boxes[seed].areaLP < 250) {
		for (int r=m_boxes[seed].rmin; r<=m_boxes[seed].rmax; r++)
			for (int c=m_boxes[seed].cmin; c<=m_boxes[seed].cmax; c++)
				if (arrayTags1[tagged(c, r)]==m_boxes[seed].id)
					id(c ,r)=col;
	//}
}


// un-defines.
#undef MaxTags
#undef MaxBoxes
#undef MaxBoxesBlobFinder
