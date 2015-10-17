#include <pebble.h>
#include "Step.h"
#include "Settings.h"

bool bInForwardPart;
int ForwardSteps;
int BackwardSteps;
	
static void data_handler(AccelData *data, uint32_t num_samples)
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "StepData %d, %d", data[0].x, data[0].y);

	if(abs(data[0].x) < 500)
		return;
	
	//See if step taken
	if(data[0].y > 300)
	{
		//Probably left handed movement forward
		if(bInForwardPart == false)
		{
			/*if(data[0].x > 500) //on left hand
			{
				APP_LOG(APP_LOG_LEVEL_DEBUG, "StepTaken left");
			}
			else
			{
				APP_LOG(APP_LOG_LEVEL_DEBUG, "StepTaken right");
			}*/

			AddStep();
			bInForwardPart = true;
		}
	}
	else
	{
		bInForwardPart = false;
	}
}

inline void StartSteps()
{
  int num_samples = 1;
  accel_data_service_subscribe(num_samples, data_handler);
	
	// Choose update rate
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
	
	bInForwardPart = false;
}

inline void EndSteps()
{
	accel_data_service_unsubscribe();
}
