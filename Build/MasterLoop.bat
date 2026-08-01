:: MasterLoop.bat
:: Infinite loop running the master server repeatedly
::
@ECHO OFF

:RUN-MASTERSERVER

ECHO.
ECHO  About to Run the Master Server....
ECHO.

cdmasterserver

ECHO.
ECHO $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
ECHO $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
ECHO  Master server is DONE
ECHO $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
ECHO $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
ECHO.

GOTO RUN-MASTERSERVER
