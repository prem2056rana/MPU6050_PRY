#include <Wire.h>
#include <GY521.h>  // Correct library to interact with MPU6050

MPU6050 mpu;

unsigned long previousTime = 0;
float timeStep = 0.01;  // This will be updated based on actual elapsed time

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

// Rotation counters
int pitchRounds = 0;
int rollRounds = 0;
int yawRounds = 0;

void setup() 
{
  Serial.begin(115200);

  Serial.println("Initialize MPU6050");

  // Initialize MPU6050 with the correct parameters
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
  mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
  
  mpu.setIntFreeFallEnabled(true);
  mpu.setIntZeroMotionEnabled(true);
  mpu.setIntMotionEnabled(true);
  
  mpu.setDHPFMode(MPU6050_DHPF_5HZ);

  // Adjust thresholds and durations
  mpu.setFreeFallDetectionThreshold(10);
  mpu.setFreeFallDetectionDuration(1);
  mpu.setMotionDetectionThreshold(2);
  mpu.setMotionDetectionDuration(1);

  mpu.setThreshold(3);  // Set the sensitivity threshold

  previousTime = millis();
}

void loop()
{
  unsigned long currentTime = millis();
  timeStep = (currentTime - previousTime) / 1000.0;  // Convert to seconds
  previousTime = currentTime;

  // Read temperature, accelerometer, and gyroscope values
  float temp = mpu.readTemperature();
  Vector normAccel = mpu.readNormalizeAccel();
  Vector normGyro = mpu.readNormalizeGyro(); // Correct method to read gyro values

  Serial.print("Temp = ");
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print("Accelerometer (m/s²) ");
  Serial.print(" Xnorm = ");
  Serial.print(normAccel.XAxis);
  Serial.print(" Ynorm = ");
  Serial.print(normAccel.YAxis);
  Serial.print(" Znorm = ");
  Serial.println(normAccel.ZAxis);

  Serial.print("Gyroscope (°/s) ");
  Serial.print(" Xnorm = ");
  Serial.print(normGyro.XAxis);
  Serial.print(" Ynorm = ");
  Serial.print(normGyro.YAxis);
  Serial.print(" Znorm = ");
  Serial.println(normGyro.ZAxis);

  // Calculate Pitch, Roll and Yaw using time-based integration
  pitch += normGyro.YAxis * timeStep;
  roll += normGyro.XAxis * timeStep;
  yaw += normGyro.ZAxis * timeStep;

  // Count full rotations for Pitch, Roll, and Yaw
  if (pitch >= 360 || pitch <= -360) {
    pitchRounds++;
    pitch = 0;  // Reset pitch after a full rotation
  }

  if (roll >= 360 || roll <= -360) {
    rollRounds++;
    roll = 0;  // Reset roll after a full rotation
  }

  if (yaw >= 360 || yaw <= -360) {
    yawRounds++;
    yaw = 0;  // Reset yaw after a full rotation
  }

  // Output only the rotation counts
  Serial.print("Pitch Rounds = ");
  Serial.print(pitchRounds);
  Serial.print(" Roll Rounds = ");
  Serial.print(rollRounds);  
  Serial.print(" Yaw Rounds = ");
  Serial.println(yawRounds);

  // Read Activities (e.g., free fall or motion detection)
  Activites act = mpu.readActivites();

  if (act.isFreeFall) {
    Serial.println("Freefall");
  } else {
    Serial.println("No Freefall");
  }

  if (act.isActivity) {
    Serial.println("Motion Detected");
  } else {
    Serial.println("No Motion");
  }

  delay(500);  // Delay between readings for better visualization
}
