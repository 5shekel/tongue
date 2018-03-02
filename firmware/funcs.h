
byte tongueDiff(int val) {
    byte servoPos;
    if ( val < 350 ) {
      servoPos = map(val, 0, 350, 20, 80); //the88p0 has less
    } else if (val > 650) {
      servoPos = map(val, 1024, 650, 150, 90);
    } else {
      servoPos = pos_middle ;// 82; //86 with the 880
    }
    return servoPos;
  }
