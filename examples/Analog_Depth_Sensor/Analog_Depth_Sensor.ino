#define ADC_PIN A0
#define SAMPLING_RATE 20
#define PRINT_RATE 800
#define BUFF_LENGTH 40

int ADC_ARRAY[BUFF_LENGTH];
int ARRAY_INDEX = 0;

const float min_milliAmps = 4.0;
const float max_milliAmps = 20.0;
const float max_depth_m = 20.0;
const float offset_depth_m = 0.0;

float milliVolts, milliAmps, depth_m;

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {
  static unsigned long SAMPLING_TIME = millis();
  static unsigned long PRINT_TIME = millis();

  if (millis() - SAMPLING_TIME > SAMPLING_RATE) {
    ADC_ARRAY[ARRAY_INDEX++] = analogRead(ADC_PIN);
    if (ARRAY_INDEX == BUFF_LENGTH) ARRAY_INDEX = 0;
    milliVolts = aveADC(ADC_ARRAY, BUFF_LENGTH) * 5000.00 / 1023.00;

    milliAmps = milliVolts / 220.0;

    float delta_milliAmps = milliAmps - min_milliAmps;

    if (delta_milliAmps < 0) {
      delta_milliAmps = min_milliAmps
    } else if (delta_milliAmps > max_milliAmps) {
      delta_milliAmps = max_milliAmps;
    }

    depth_m = (max_depth_m / (max_milliAmps - min_milliAmps)) * delta_milliAmps + offset_depth_m;

    SAMPLING_TIME = millis();
  }

  if (millis() - PRINT_TIME > PRINT_RATE) {
    Serial.print("mV Ave:");
    Serial.print(milliVolts, 2);
    Serial.print("\t mA Ave:");
    Serial.print(milliAmps, 2);
    Serial.print("\t depth m:");
    Serial.print(depth_m, 2);
    Serial.print("\n");
    PRINT_TIME = millis();
  }
}

double aveADC(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;

  if (number < 5) {
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0];
      max = arr[1];
    } else {
      min = arr[1];
      max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;
          max = arr[i];
        } else {
          amount += arr[i];
        }
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}