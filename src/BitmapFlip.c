#include <pebble.h>
#include "BitmapFlip.h"
	
void CopyBitmapData(GBitmap *src, GBitmap *dest, int offset)
{
  int height = gbitmap_get_bounds(src).size.h;
  int bytesPerRow = gbitmap_get_bytes_per_row(src);
	
  uint8_t *simageData = gbitmap_get_data(src);
	uint8_t *dimageData = gbitmap_get_data(dest);
	
	//return;

	memcpy(&(dimageData[offset]), simageData, sizeof(uint8_t) * bytesPerRow * height);
	
	GColor *sPal = gbitmap_get_palette(src);
	GColor *dPal = gbitmap_get_palette(dest);
		
	//memcpy(dPal, sPal, 16);
	
	//Only 4&2bit palette for now
	GBitmapFormat format = gbitmap_get_format(src);
	switch(format)
	{
		/*case GBitmapFormat8Bit:
		{
		} break;*/

		case GBitmapFormat4BitPalette:
		{
			memcpy(dPal, sPal, 16);
    } break;

		case GBitmapFormat2BitPalette:
		{
			memcpy(dPal, sPal, 4);
    } break;
		
		case GBitmapFormat1BitPalette:
		{
			memcpy(dPal, sPal, 2);
    } break;
		
		default:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tried to copy unsupported type %d", format);
			return;
		}
	}
}

void FlipBitmap(GBitmap *image)
{
  int width = gbitmap_get_bounds(image).size.w, height = gbitmap_get_bounds(image).size.h;
  GBitmapFormat format = gbitmap_get_format(image);
  int bytesPerRow = gbitmap_get_bytes_per_row(image);
  uint8_t *imageData = gbitmap_get_data(image);
  uint8_t tempVal;
//  int OriginalWidth = width;
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Flipping image %d %d %d %d", format, width, height, bytesPerRow);
  
  for(int rowNo = 0; rowNo < height; ++rowNo)
  {
      int rowOffset = bytesPerRow * rowNo;
    
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Flipping 8bit row=%d", rowNo);
      switch(format)
      {
      case GBitmapFormat8Bit:
      {
        for(int pixelNo = 0; pixelNo < (width / 2); ++pixelNo)
         {
          //APP_LOG(APP_LOG_LEVEL_DEBUG, "Flipping 8bit pixel %d with pixel %d", pixelNo, width - (pixelNo + 1));
          tempVal = imageData[pixelNo + rowOffset];
          imageData[rowOffset + pixelNo] = imageData[rowOffset + width - (pixelNo + 1)];
          imageData[rowOffset + width - (pixelNo + 1)] = tempVal;
        }
      } break;
        
/*      case GBitmapFormat4BitPalette:
      {
        width = OriginalWidth;
        width /= 2; //2 pixels per byte
        
        for(int pixelNo = 0; pixelNo < (width / 2); ++pixelNo)
        {
          if(OriginalWidth % 2 == 0)
          {
            //Get top bits of first pixel and put into bottom bits of last
            tempVal = imageData[rowOffset + pixelNo] >> 4;
            imageData[rowOffset + pixelNo] = (imageData[rowOffset + pixelNo] & 15) | ((imageData[rowOffset + width - (pixelNo + 1)] << 4) & 240);
            imageData[rowOffset + width - (pixelNo + 1)] = (imageData[rowOffset + width - (pixelNo + 1)] & 240) | (tempVal);

            //if not divisible by 4 then centre byte is by itself. So skip next bit or you'll re-reverse it
            if((pixelNo == (width / 2) && OriginalWidth % 4 != 0) == false)
            {
              //Get bottom bits of first pixel and put into top bits of last
              tempVal = (imageData[rowOffset + pixelNo] << 4) & 240;
              imageData[rowOffset + pixelNo] = (imageData[rowOffset + pixelNo] & 240) | (imageData[rowOffset + width - (pixelNo + 1)] >> 4);
              imageData[rowOffset + width - (pixelNo + 1)] = (imageData[rowOffset + width - (pixelNo + 1)] & 15) | (tempVal);
            }
          }
          else
          {
            //Get top bits of first pixel and put into top bits of last
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "Flipping 4bit row=%d, pixel %d with pixel %d", rowNo, pixelNo, width - (pixelNo));
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "Old val %d %d %d", imageData[rowOffset + pixelNo], imageData[rowOffset + width - (pixelNo)], imageData[rowOffset + width - (pixelNo + 1)]);
            tempVal = imageData[rowOffset + pixelNo] & 240;
            imageData[rowOffset + pixelNo] = (imageData[rowOffset + pixelNo] & 15) | (imageData[rowOffset + width - (pixelNo)] & 240);
            imageData[rowOffset + width - (pixelNo)] = (imageData[rowOffset + width - (pixelNo)] & 15) | (tempVal);

            //APP_LOG(APP_LOG_LEVEL_DEBUG, "New val %d %d %d", imageData[rowOffset + pixelNo], imageData[rowOffset + width - (pixelNo)], imageData[rowOffset + width - (pixelNo + 1)]);

            //Get bottom bits of first pixel and put into bottom bits of second last
            if((pixelNo == (width / 2) && (OriginalWidth - 1) % 4 != 0) == false)
            {
              tempVal = imageData[rowOffset + pixelNo] & 15;
              imageData[rowOffset + pixelNo] = (imageData[rowOffset + pixelNo] & 240) | (imageData[rowOffset + width - (pixelNo + 1)] & 15);
              imageData[rowOffset + width - (pixelNo + 1)] = (imageData[rowOffset + width - (pixelNo + 1)] & 240) | (tempVal);
            }
            
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "Final val %d %d %d", imageData[rowOffset + pixelNo], imageData[rowOffset + width - (pixelNo)], imageData[rowOffset + width - (pixelNo + 1)]);
          }
        }
      } break;*/

      default:
      {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Tried to flip unsupported type %d", format);
        return; //no flipping of unsurported types
      }
    }
  }
}

void PackBitmap(int ResourceId, GBitmap* DestBitmap)
{
		GBitmap *TestB;
	
	  TestB = gbitmap_create_with_resource(ResourceId);
		CopyBitmapData(TestB, DestBitmap, 0);
		gbitmap_destroy(TestB);
}