PROGRAM Task1
  VAR
    Start AT %IX0.1 : BOOL;
    Stop AT %IX0.0 : BOOL;
  END_VAR
  VAR
    Latch : BOOL;
  END_VAR
  VAR
    LED AT %QX0.0 : BOOL;
  END_VAR

  Latch := NOT(Stop) AND (Latch OR Start);
  LED := Latch;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : Task1;
  END_RESOURCE
END_CONFIGURATION
