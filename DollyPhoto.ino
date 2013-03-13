/**********************************************************
 * Dolly Photo Arduino
 *
 * Data creazione 25 febbraio 2013
 * Ultima modifica 13 marzo 2013
 * 
 * autore: Mauro Alfieri
 * web:    mauroalfieri.it
 * tw:     @mauroalfieri
 *
/**********************************************************/

#include <LiquidCrystal.h>

/**********************************************************/

#define BUTTON A0
#define BACKLIGHT 10
#define MOTDTIME 2000
#define LCDTIME  3000
#define SETMODETIME 2000
#define TIMESCATTO 500
#define DEBUG

/**********************************************************/

#define PWMA 3
#define PWMB 11
#define DIRA 12
#define DIRB 13
#define TIMEFASE 35
#define SCATTO 2

/**********************************************************/

char line0[16] = "Dolly Foto ver1";
char line1[16] = "mauroalfieri.it";

/**********************************************************/

int SELECT[]     =  {720,760};
int LEFT[]       =  {480,520};
int RIGTH[]      =  {0,20};
int UP[]         =  {120,160};
int DOWN[]       =  {300,350};

int buttonPress  = 0;
int nFase        = 0;
int scatto       = 0;
int passo        = 0;

/**********************************************************/

unsigned long timeToBlacklight  =  0;
unsigned long timeToSetMode     =  0;
unsigned long timeSetting       =  0;
unsigned long timeDelay         =  0;

/**********************************************************/

boolean blacklightStatus = false;

/**********************************************************/

byte symbolselect[8] = {B00001,B00011,B00111,B01111,B00111,B00011,B00001};
byte symbolplay[8]   = {B10000,B11000,B11100,B11110,B11100,B11000,B10000};
byte symbolstop[8]   = {B00000,B11111,B11111,B11111,B11111,B11111,B00000};
byte symbolpause[8]  = {B00000,B10001,B10001,B10001,B10001,B10001,B00000};

/**********************************************************/
char* principale[4]   =  {"Impostazioni","Verifica","Reset Carrello","Avvio"};
char* secondario[10]  =  {"Numero passi","per scatto","Intervallo tra","i passi","Tempo di ALT","prima","Tempo di ALT","dopo","Numero scatti","totali"};
char buffer[16];

/**********************************************************/

int passiXscatto      = 0;
int intervalloScatto  = 0;
int tempoAltPrima     = 0;
int tempoAltDopo      = 0;
int numeroScatti      = 0;

/**********************************************************/

LiquidCrystal lcd(8,9,4,5,6,7);

/**********************************************************/

void setup() {

  #ifdef DEBUG
    Serial.begin( 9600 );
    Serial.println("Avvio");
  #endif

  /**********************************************************/

  pinMode( BACKLIGHT,OUTPUT );
  digitalWrite( BACKLIGHT,LOW );

  /**********************************************************/

  pinMode( SCATTO,OUTPUT );
  digitalWrite( SCATTO,LOW );

  /**********************************************************/

  lcd.begin(16, 2);
  lcd.createChar(1, symbolselect);
  lcd.createChar(2, symbolplay);
  lcd.createChar(3, symbolstop);
  lcd.createChar(4, symbolpause);

  /**********************************************************/

  digitalWrite( BACKLIGHT,HIGH );
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
  delay( MOTDTIME );
  lcd.clear();
  lcdBlacklightOff();
}

/**********************************************************/

void loop() {

  /**********************************************************/

  #ifdef DEBUG
    Serial.print( " Button: " );           Serial.print( analogRead( BUTTON ) );
    Serial.print( " TimeToBlecklight " );  Serial.print( timeToBlacklight );
    Serial.print( " TimeToSetHour " );     Serial.println( timeToSetMode );
  #endif

  /**********************************************************/

  if ( ctrlButton( analogRead( BUTTON ) ) == 0 || ctrlButton( analogRead( BUTTON ) ) > 1 ) { timeToSetMode = millis(); }
  if (timeToSetMode > 0 && SETMODETIME < (millis() - timeToSetMode) ) { menuMode(); timeToSetMode = 0; }

  /**********************************************************/

  buttonPress = ctrlButton( analogRead( BUTTON ) );

  /**********************************************************/

  if (  (blacklightStatus) && LCDTIME < (millis() - timeToBlacklight) ) { lcdBlacklightOff(); lcd.clear(); }

  /**********************************************************/
  
  char line0[16] = " in attesa di  ";
  char line1[16] = "    comandi ...";
  
  /**********************************************************/
  
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
  
  /**********************************************************/
}

/**********************************************************/

int ctrlButton( int button ) {

  buttonPress = 0;
  if ( SELECT[0] <= button && button <= SELECT[1] )  { buttonPress = 1; }
  if ( LEFT[0] <= button && button <= LEFT[1] )      { buttonPress = 2; }
  if ( RIGTH[0] <= button && button <= RIGTH[1] )    { buttonPress = 3; }
  if ( UP[0] <= button && button <= UP[1] )          { buttonPress = 4; }
  if ( DOWN[0] <= button && button <= DOWN[1] )      { buttonPress = 5; }

  if (buttonPress > 0) {
    digitalWrite( BACKLIGHT,HIGH ); 
    blacklightStatus = true; 
    timeToBlacklight = millis(); 
  }
  
  return buttonPress;
}

/**********************************************************/

void lcdBlacklightOff() {
  digitalWrite( BACKLIGHT,LOW );
  blacklightStatus = false; 
  timeToBlacklight = 0;
}

/**********************************************************/

void lcdBlacklightOn() {
  digitalWrite( BACKLIGHT,HIGH );
  blacklightStatus = true; 
  timeToBlacklight = millis();
}

/**********************************************************/

void menuMode() {
  
  /**********************************************************/

  #ifdef DEBUG
    Serial.println( "Menu Mode" );
  #endif

  /**********************************************************/

  boolean setMode  = true;
  int setModeLevel = 0;
  timeSetting      = 0;
  lcd.clear();
    
  /**********************************************************/
  
  delay( 1000 );
    
  /**********************************************************/  
    
  while ( setMode ) {

    /**********************************************************/

    if ( ctrlButton( analogRead( BUTTON ) ) != 0 ) { timeSetting = millis(); }

    if ( (ctrlButton( analogRead( BUTTON )) == 5 ) && setModeLevel < 3 ) { lcd.clear(); setModeLevel++; }
    if ( (ctrlButton( analogRead( BUTTON )) == 4 ) && setModeLevel > 0 ) { lcd.clear(); setModeLevel--; }

    if ( (setModeLevel % 2) == 0 ) { 
      lcd.setCursor(0,0);
      lcd.print( principale[setModeLevel] );
      lcd.setCursor(0,1);
      lcd.print( principale[(setModeLevel+1)] );  
      
      lcd.setCursor(15,0);
      lcd.write(1);
      lcd.setCursor(15,1);
      lcd.print(" ");      
    } else { 
      lcd.setCursor(0,0);
      lcd.print( principale[(setModeLevel-1)] );
      lcd.setCursor(0,1);
      lcd.print( principale[setModeLevel] );  
    
      lcd.setCursor(15,0);
      lcd.print(" ");
      lcd.setCursor(15,1);
      lcd.write(1);  
    }
    
    if ( ctrlButton( analogRead( BUTTON )) == 1 ) { 
      if ( setModeLevel == 0 ) { impostazioni(); }
      if ( setModeLevel == 1 ) { verifica();     }
      if ( setModeLevel == 2 ) { reset();        }
      if ( setModeLevel == 3 ) { avvio();        }
  
      setMode = false;
      timeSetting = 0;
    }

    if (timeSetting > 0 && (SETMODETIME*2) < (millis() - timeSetting) ) { setMode = false; }
    delay(200);
  }

  /**********************************************************/
  
  lcd.clear();
}

/**********************************************************/

void impostazioni() {
  
  /**********************************************************/

  #ifdef DEBUG
    Serial.println( "Menu Impostazioni" );
  #endif

  /**********************************************************/

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "Menu " );
  lcd.setCursor(0,1);
  lcd.print( "Impostazioni" );    
      
  /**********************************************************/

  delay( SETMODETIME );
  
  /**********************************************************/

  boolean impostazioniMode  = true;
  int setModeLevel = 0;
  lcd.clear();
    
  /**********************************************************/
  
  while ( impostazioniMode ) {
  
    /**********************************************************/
  
    if ( (ctrlButton( analogRead( BUTTON )) == 5 ) && setModeLevel < 8 ) { lcd.clear(); setModeLevel = (setModeLevel+2); }
    if ( (ctrlButton( analogRead( BUTTON )) == 4 ) && setModeLevel > 0 ) { lcd.clear(); setModeLevel = (setModeLevel-2); }
    if (  ctrlButton( analogRead( BUTTON )) == 1 )                       { impostazioniMode = false; }
  
    /**********************************************************/

    #ifdef DEBUG
      Serial.print( "setMenuLevel: " );
      Serial.println( setModeLevel );
    #endif
  
    /**********************************************************/


    switch ( setModeLevel ) {
      
      case 0:
        if ( (ctrlButton( analogRead( BUTTON )) == 3 ) && passiXscatto <= 99999)  { passiXscatto++; lcd.clear(); }
        if ( (ctrlButton( analogRead( BUTTON )) == 2 ) && passiXscatto >= 1)      { passiXscatto--; lcd.clear();  }
        
        sprintf(buffer, "%s %d", secondario[(setModeLevel+1)], passiXscatto);
      break;
      
      case 2:
        if ( (ctrlButton( analogRead( BUTTON )) == 3 ) && intervalloScatto <= 9999999)  { intervalloScatto++; lcd.clear();  }
        if ( (ctrlButton( analogRead( BUTTON )) == 2 ) && intervalloScatto >=     1)    { intervalloScatto--; lcd.clear();  }

        sprintf(buffer, "%s %ds", secondario[(setModeLevel+1)], intervalloScatto);
      break;
      
      case 4:
        if ( (ctrlButton( analogRead( BUTTON )) == 3 ) && tempoAltPrima <= 9999999)  { tempoAltPrima++; lcd.clear();  }
        if ( (ctrlButton( analogRead( BUTTON )) == 2 ) && tempoAltPrima >=     1)    { tempoAltPrima--; lcd.clear();  }

        sprintf(buffer, "%s %ds", secondario[(setModeLevel+1)], tempoAltPrima);
      break;
      
      case 6:
        if ( (ctrlButton( analogRead( BUTTON )) == 3 ) && tempoAltDopo <= 9999999)  { tempoAltDopo++; lcd.clear();  }
        if ( (ctrlButton( analogRead( BUTTON )) == 2 ) && tempoAltDopo >=     1)    { tempoAltDopo--; lcd.clear();  }

        sprintf(buffer, "%s %ds", secondario[(setModeLevel+1)], tempoAltDopo);
      break;
      
      case 8:
        if ( (ctrlButton( analogRead( BUTTON )) == 3 ) && numeroScatti <= 9999999)  { numeroScatti++; lcd.clear();  }
        if ( (ctrlButton( analogRead( BUTTON )) == 2 ) && numeroScatti >=     1)    { numeroScatti--; lcd.clear();  }

        sprintf(buffer, "%s %d", secondario[(setModeLevel+1)], numeroScatti);
      break;
      
    }
  
    /**********************************************************/
  
    lcd.setCursor(0,0);
    lcd.print( secondario[setModeLevel] );
    lcd.setCursor(0,1);
    lcd.print( buffer );        

    /**********************************************************/
    
    delay( 200 );

  }
  
  /**********************************************************/
  
  lcd.clear();
  
}

/**********************************************************/

void verifica() {
  /**********************************************************/

  #ifdef DEBUG
    Serial.println( "Menu Verifica" );
  #endif

  /**********************************************************/

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "Menu " );
  lcd.setCursor(0,1);
  lcd.print( "Verifica" );    
      
  /**********************************************************/

  delay( SETMODETIME );
  
  /**********************************************************/

  boolean verificaMode  = true;
  int setModeLevel = 0;
  lcd.clear();
    
  /**********************************************************/
  
  while ( verificaMode ) {
  
    /**********************************************************/
  
    if ( (ctrlButton( analogRead( BUTTON )) == 5 ) && setModeLevel < 8 ) { lcd.clear(); setModeLevel = (setModeLevel+2); }
    if ( (ctrlButton( analogRead( BUTTON )) == 4 ) && setModeLevel > 0 ) { lcd.clear(); setModeLevel = (setModeLevel-2); }
    if (  ctrlButton( analogRead( BUTTON )) == 1 )                       { verificaMode = false; }
  
    /**********************************************************/

    #ifdef DEBUG
      Serial.print( "setMenuLevel: " );
      Serial.println( setModeLevel );
    #endif
  
    /**********************************************************/


    switch ( setModeLevel ) {
      
      case 0:
        sprintf(buffer, "%s %d", secondario[(setModeLevel+1)], passiXscatto);
      break;
      
      case 2:
        sprintf(buffer, "%s %ds", secondario[(setModeLevel+1)], intervalloScatto);
      break;
      
      case 4:
        sprintf(buffer, "%s %ds", secondario[(setModeLevel+1)], tempoAltPrima);
      break;
      
      case 6:
        sprintf(buffer, "%s %ds", secondario[(setModeLevel+1)], tempoAltDopo);
      break;
      
      case 8:
        sprintf(buffer, "%s %d", secondario[(setModeLevel+1)], numeroScatti);
      break;
      
    }
  
    /**********************************************************/
  
    lcd.setCursor(0,0);
    lcd.print( secondario[setModeLevel] );
    lcd.setCursor(0,1);
    lcd.print( buffer );        

    /**********************************************************/
    
    delay( 200 );

  }
  
  /**********************************************************/
  
  lcd.clear();
  
}

/**********************************************************/

void reset() {
  
  /**********************************************************/

  #ifdef DEBUG
    Serial.println( "Menu Reset" );
  #endif

  /**********************************************************/

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "Menu " );
  lcd.setCursor(0,1);
  lcd.print( "Reset" );    
      
  /**********************************************************/

  delay( SETMODETIME );
  
  /**********************************************************/

  boolean resetMode  = true;
  lcd.clear();
    
  /**********************************************************/
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "LEFT antiorario" );
  lcd.setCursor(0,1);
  lcd.print( "RIGTH orario" );    
      
  /**********************************************************/

  while ( resetMode ) {

    /**********************************************************/
  
    if ( ctrlButton( analogRead( BUTTON )) == 3 )  { nFase++; }
    if ( ctrlButton( analogRead( BUTTON )) == 2 )  { nFase--; }
    if ( ctrlButton( analogRead( BUTTON )) == 1 )  { resetMode = false; }
  
    /**********************************************************/
    
    nFase = fase( nFase );
    
    /**********************************************************/
    
    delay( 100 );
  }
  
  /**********************************************************/
  
  stop();
  
  /**********************************************************/
  
  lcd.clear();
}

/**********************************************************/

void avvio() {
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "Menu " );
  lcd.setCursor(0,1);
  lcd.print( "Avvio" );    
  
  /**********************************************************/

  delay( SETMODETIME );
  
  /**********************************************************/
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "Go " );
  lcd.setCursor(3, 0);
  lcd.write(4);        // PAUSA
  
  lcd.setCursor(0, 1);
  lcd.write(3);        // STOP
  lcd.setCursor(5, 1);
  lcd.write(2);        // PLAY
  
  /**********************************************************/
  
  if ( !pausa() ) return;
  
  /**********************************************************/
  
  for ( scatto=0; scatto <= (numeroScatti+1); scatto++ ) {
    
    lcd.setCursor(7, 0);
    lcd.print( "scatto:" );
    lcd.setCursor(14, 0);
    lcd.print( scatto );
    
    /**********************************************************/    
    
    for ( int passo=1; passo <= passiXscatto; passo++ ) {
      
      lcd.setCursor(7, 1);
      lcd.print( "passo:" );
      lcd.setCursor(13, 1);
      lcd.print( passo );  
      
      if ( ctrlButton( analogRead( BUTTON )) == 3 ) { if ( !pausa() ) return; }

      nFase = fase( nFase );
      nFase++;
      int _intervallo = (intervalloScatto * 1000);
      if ( !ctrlDelay( _intervallo )) return;
      
    }
    
    /**********************************************************/
    
    int _tempo = (tempoAltPrima * 1000);
    if ( !ctrlDelay( _tempo )) return;
    
    /**********************************************************/
    
    digitalWrite( SCATTO, HIGH );
    delay( TIMESCATTO );
    digitalWrite( SCATTO, LOW );
  
    /**********************************************************/

    lcd.setCursor(7, 0);
    lcd.print( "scatto:" );
    lcd.setCursor(14, 0);
    lcd.print( scatto );    
    
    /**********************************************************/    

    _tempo = (tempoAltDopo * 1000);
    if ( !ctrlDelay( _tempo )) return;
    
    /**********************************************************/
 
  }

  /**********************************************************/
    
}

/**********************************************************/

boolean pausa() {
  
  lcdBlacklightOn();
  
  /**********************************************************/
  
  boolean play  =  false;
  
  /**********************************************************/    

  lcd.setCursor(3, 0);
  lcd.write(4);    // PAUSA
  
  lcd.setCursor(5, 1);
  lcd.write(2);    // PLAY

  /**********************************************************/    

  while ( !play ) {
   if ( ctrlButton( analogRead( BUTTON )) == 3 ) { 
      lcd.setCursor(3, 0);
      lcd.write(2);    // PLAY
      
      lcd.setCursor(5, 1);
      lcd.write(4);    // PAUSA
      delay( 500 );
      play = true;
    }
    
    if ( ctrlButton( analogRead( BUTTON )) == 2 ) {
      stop();
      play = true;
      return false;
    }
    delay( 100 );
  }
  
  /**********************************************************/ 
  
  return true;  
}

/**********************************************************/

boolean ctrlDelay(int tempo) {

  // lcdBlacklightOff(); // Decommenta per risparmio energetico
  timeDelay = millis();
  boolean dtime = true;
  
  #ifdef DEBUG
      Serial.print( "Delay: " );
      Serial.print( tempo );
      Serial.print( " Millis: " );
      Serial.print( timeDelay );
      Serial.print( " \n " );
  #endif
  
  while ( dtime ) {
    if ( tempo < (millis()-timeDelay)) { dtime = false; }
         
    if ( ctrlButton( analogRead( BUTTON )) == 3 ) { delay( 500 ); pausa(); }
    if ( ctrlButton( analogRead( BUTTON )) == 2 ) {
      return false;
      stop();
      dtime = false;
    }
    Serial.print( (millis()-timeDelay) );
    Serial.print( "\n" );
  }
  
  return true;
}

/**********************************************************/

int fase( int nFase ) {

  if ( nFase > 4 ) { nFase = 1; }
  if ( nFase < 1 ) { nFase = 4; }
  
  #ifdef DEBUG
      Serial.print( "fase() - nFase: " );
      Serial.println( nFase );
  #endif
  
  switch( nFase ) {
  case 1:
    digitalWrite(DIRA, HIGH);
    digitalWrite(DIRB, LOW);
    digitalWrite(PWMA, HIGH);
    digitalWrite(PWMB, LOW);
    break;

  case 2:
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, HIGH);
    digitalWrite(PWMA, LOW);
    digitalWrite(PWMB, HIGH);
    break;

  case 3:
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, LOW);
    digitalWrite(PWMA, HIGH);
    digitalWrite(PWMB, LOW);
    break;

  case 4:
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, LOW);
    digitalWrite(PWMA, LOW);
    digitalWrite(PWMB, HIGH);
    break;
  }
  
  delay(TIMEFASE);
  
  return nFase;
}

/**********************************************************/

int stop() {
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
}

/**********************************************************/
