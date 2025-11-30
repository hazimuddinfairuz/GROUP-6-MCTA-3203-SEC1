TYPE
  LOGLEVEL : (CRITICAL, WARNING, INFO, DEBUG) := INFO;
END_TYPE

FUNCTION_BLOCK LOGGER
  VAR_INPUT
    TRIG : BOOL;
    MSG : STRING;
    LEVEL : LOGLEVEL := INFO;
  END_VAR
  VAR
    TRIG0 : BOOL;
  END_VAR

  IF TRIG AND NOT TRIG0 THEN
  {{
   LogMessage(GetFbVar(LEVEL),(char*)GetFbVar(MSG, .body),GetFbVar(MSG, .len));
  }}
  END_IF;
  TRIG0:=TRIG;
END_FUNCTION_BLOCK


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
