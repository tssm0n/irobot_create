
void checkIr(struct cm_state* cmState){
	short manualDriveChanged = 0;	
	switch(sensors[SenIRChar]){
		case 144:
			addToI2cBuffer(LED2_ON);
			break;
		case IR_MAX:
			addToI2cBuffer(LED2_OFF);
                        statusPin1Value = LOW;
                        moveServo(SERVO_HALF_RIGHT);
                        delay(1000);
                        moveServo(SERVO_CENTER_POS);                        
			break;
		case IR_CLEAN:
			doDrive();
			break;
		case IR_SPOT:
                        addToI2cBuffer(LED2_ON);
                        statusPin1Value = HIGH;
                        moveServo(SERVO_HALF_LEFT);
                        delay(1000);
                        moveServo(SERVO_CENTER_POS);
			//doFollow();
			break;
		case IR_RIGHT:
			if(cmState->left == 0 && cmState->right == 0){
				if(cmState->spin == SPIN_RIGHT){
					cmState->spinSpeed += MANUAL_INTERVAL;
				} else {
					cmState->spinSpeed = MANUAL_INTERVAL;
					cmState->spin = SPIN_RIGHT;
				}
			} else {
				if(cmState->left == 500){
					cmState->right -= MANUAL_INTERVAL;
				} else {
					cmState->left += MANUAL_INTERVAL;
				}
			}
			manualDriveChanged = 1;
			break;
		case IR_LEFT:
			if(cmState->left == 0 && cmState->right == 0){
				if(cmState->spin == SPIN_LEFT){
					cmState->spinSpeed += MANUAL_INTERVAL;
				} else {
					cmState->spinSpeed = MANUAL_INTERVAL;
					cmState->spin = SPIN_LEFT;
				}
			} else {
				if(cmState->right == 500){
					cmState->left -= MANUAL_INTERVAL;
				} else {
					cmState->right += MANUAL_INTERVAL;
				}
			}
			manualDriveChanged = 1;
			break;
		case IR_FORWARD:
			cmState->spin = SPIN_NONE;
			cmState->spinSpeed = 0;
			cmState->left += MANUAL_INTERVAL;
			cmState->right += MANUAL_INTERVAL;
			manualDriveChanged = 1;
			break;
		case IR_PAUSE:
			cmState->left = 0;
			cmState->right = 0;
                        cmState->spin = SPIN_NONE;
			manualDriveChanged = 1;
			break;
	}
	if(manualDriveChanged){
		if(cmState->left > 500){
			cmState->left = 500;
		}
		if(cmState->right > 500){
			cmState->right = 500;
		}
		switch(cmState->spin){
			case SPIN_NONE:
				driveDirect(cmState->right, cmState->left);
				break;
			case SPIN_LEFT:
				drive(cmState->spinSpeed, 0x0001);
				break;
			case SPIN_RIGHT:
				drive(cmState->spinSpeed, 0xFFFF);
				break;
		}
	}
}
