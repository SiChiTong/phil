#include <phil/phil_rio/phil_rio.h>
#include <RobotMap.h>
#include <Timer.h>
#include <SPI.h>
#include <Subsystems/DriveBase.h>
#include <Commands/JoystickTeleop.h>
#include <Victor.h>

DriveBase::DriveBase() :
    frc::Subsystem("DriveBase"), left_motor(nullptr), right_motor(nullptr), left_encoder(nullptr), right_encoder(
        nullptr), ahrs(nullptr) {
  left_motor = new frc::Victor(RobotMap::kLeftMotor);
  right_motor = new frc::Victor(RobotMap::kRightMotor);

  left_encoder = new frc::Encoder(RobotMap::kLeftEnocderA, RobotMap::kLeftEnocderB);
  right_encoder = new frc::Encoder(RobotMap::kRightEnocderA, RobotMap::kRightEnocderB);

  ahrs = new AHRS(SPI::Port::kMXP);

  phil::Phil::GetInstance()->GiveSensors(left_encoder, right_encoder, ahrs);
}

void DriveBase::InitDefaultCommand() {
  SetDefaultCommand(new JoystickTeleop());
}

void DriveBase::Stop() {
  left_motor->StopMotor();
  right_motor->StopMotor();
}

void DriveBase::SetSpeed(double left_speed, double right_speed) {
  left_motor->Set(left_speed);
  right_motor->Set(-right_speed);
}

