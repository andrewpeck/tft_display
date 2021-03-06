#ifndef SCREEN_H
#define SCREEN_H

#include <CleO.h>

#define MY_ORANGE   0xfa7626UL
#define MY_MAGENTA  0xff0086UL
#define MY_RED      0xff6666UL
#define MY_GREEN    0x89df87UL
#define MY_BLUE     0x007fc8UL
#define MY_DARKBLUE 0x004166UL
#define MY_PINK     0xe9d3ebUL
#define MY_BLACK    0x000000UL
#define MY_WHITE    0xffffffUL
#define MY_PURPLE   0xb3b3ccUL

 extern uint16_t read_voltage_counts_average[2];
 extern uint16_t read_current_counts_average[2];

extern uint16_t set_decivolts [2]    ;  // Set Voltage IN deci-Volts
extern uint32_t set_microamps [2]    ;  // Set Current IN micro-Amps

extern bool ovp_ok    [2]  ;
extern bool ovc_ok    [2]  ;
extern bool status_ok [2] ;

extern bool output_on [2];

void setupScreen () ;
void shortPress (bool *press_detected);
void press (uint8_t debounce_time, uint8_t *high_cnt, uint8_t *low_cnt, bool *press_allow, bool *press_detected);
void printCurrentTag () ;
void setDecimalPoint();
void moveCursorLeft();
void moveCursorRight();
void updateSetStrings (uint8_t panel) ;
void setDigitValue (uint8_t value, uint8_t panel);
void buildStatus (int panel);
void processLongPress() ;
void processShortPress() ;
void buildKeypad (int keypad);
void buildKey    (int x, int y, int box_size, int tag, char* text) ;
void processButtons ()  ;
void updateScreen();



int screenWidth = 480, screenHeight = 320;
int windowWidth = 240, windowHeight = 320;


int bounding_margin = 20;
uint32_t keypad_taglist=0;

bool keypad_enabled [2] = {0,0};

uint32_t keypad_color       = MY_PURPLE;
uint32_t keypad_text_color  = MY_BLACK;

uint32_t display_text_color = MY_BLACK;

uint32_t background_color   = MY_PINK;

uint32_t line_color         = MY_DARKBLUE;

uint32_t color_good         = MY_GREEN;
uint32_t color_bad          = MY_RED;

uint32_t color_on           = MY_GREEN;
uint32_t color_off          = MY_RED;

uint32_t set_color          = keypad_color;
uint32_t touch_color        = MY_BLUE;

int16_t font;

enum {
    tag_null    , // 0
    tag_0       , // 1
    tag_1       , // 2
    tag_2       , // 3
    tag_3       , // 4
    tag_4       , // 5
    tag_5       , // 6
    tag_6       , // 7
    tag_7       , // 8
    tag_8       , // 9
    tag_9       , // 10
    tag_point   , // 11
    tag_left    , // 12
    tag_right   ,//  13

    tag_power0   , // 13
    tag_voltage0 , // 14
    tag_current0, // 15

    tag_power1   , // 13
    tag_voltage1 , // 14
    tag_current1  // 15
};

int last_dur = 0;

uint8_t   short_high_cnt       = 0;
uint8_t   short_low_cnt        = 0;
bool      short_press_allow    = 0;
bool      short_press_detected = 0;

uint8_t   long_high_cnt       = 0;
uint8_t   long_low_cnt        = 0;
bool      long_press_allow    = 0;
bool      long_press_detected = 0;

int16_t x, y, dur, current_tag;

int tag_powers   [2] = {tag_power0, tag_power1};
int tag_currents [2] = {tag_current0, tag_current1};
int tag_voltages [2] = {tag_voltage0, tag_voltage1};

uint8_t cursor_index     = 0;
uint8_t decimal_index    = 1;
uint8_t decimal_position = 2;

char set_voltage_str       [] = "    V";
char set_current_str       [] = "00.0mA";

char set_voltage_highlight [] = "    V";
char set_current_highlight [] = "00.0mA";

bool voltage_set_mode = 0;
bool current_set_mode = 0;

uint16_t set_voltage_buffer [2] = {0,0}; // set voltage (in     Volts)
uint16_t set_current_buffer [2] = {0,0}; // set current (in    100uA units)

void setupScreen ()
{
}


void shortPress (bool *press_detected)
{
    if (dur==1 && last_dur==0 && !*press_detected) {
        *press_detected = 1;
        SerialUSB.println("Short press!");
    }
    else {
        *press_detected = 0;
    }
}

void press (uint8_t debounce_time, uint8_t *high_cnt, uint8_t *low_cnt, bool *press_allow, bool *press_detected)
{
    if (dur==1 && last_dur==1 && *press_allow) {
        *high_cnt += 1;
    }

    if (dur==0 && last_dur==0 && !*press_allow)  {
        *low_cnt += 1;
    }

    if (*low_cnt > debounce_time/2) {
        *press_allow = 1;
        *low_cnt     = 0;
        *high_cnt    = 0;
    }

    if (*press_allow && *high_cnt > debounce_time) {
        SerialUSB.println("Long press!");
        *press_allow    = 0;
        *press_detected = 1;
    }
    else
    {
        *press_detected = 0;
    }
}

void printCurrentTag ()
{
    if (current_tag!=0 && current_tag!=255) {
        SerialUSB.print("tag: ");
        SerialUSB.print(current_tag);
        SerialUSB.print(" dur: ");
        SerialUSB.print(dur);
        SerialUSB.print(" x: ");
        SerialUSB.print(x);
        SerialUSB.print(" y: ");
        SerialUSB.print(y);
        SerialUSB.print("\n");
    }
}

void setDecimalPoint()
{
}

void moveCursorLeft()
{
    if (current_set_mode && cursor_index==3) // skip the decimal place
        cursor_index--;

    if (cursor_index>0)
        cursor_index--;
}

void moveCursorRight()
{
    int max=3;

    if (current_set_mode && cursor_index==1) // skip the decimal place
        cursor_index++;

    if (cursor_index<max)
        cursor_index++;
    else
        cursor_index=0;
}

void updateSetStrings (uint8_t panel)
{
    // need to breakdown the set current for decimal representation
    int characteristic = set_current_buffer[panel] / 10;
    int mantissa       = set_current_buffer[panel] % 10;

    sprintf(set_voltage_str,         "%04luV",         set_voltage_buffer[panel]);
    sprintf(set_current_str,         "%02lu.%01lumA",  characteristic, mantissa);
}

void setDigitValue (uint8_t value, uint8_t panel)
{
    // take value
    // convert to string
    char thischar [] = " ";
    sprintf(thischar,   "%1d",  value);

    char* end;

    updateSetStrings(panel);

    if (voltage_set_mode) {
        SerialUSB.print("Read in character: " );
        SerialUSB.print(thischar[0]);
        SerialUSB.print("\n" );

        SerialUSB.print("Old string ");
        SerialUSB.print(set_voltage_str);
        SerialUSB.print("\n");

        SerialUSB.print("Cursor index ");
        SerialUSB.print(cursor_index);
        SerialUSB.print("\n");

        set_voltage_str[cursor_index] = thischar[0]; // swap this index

        SerialUSB.print("New string ");
        SerialUSB.print(set_voltage_str);
        SerialUSB.print("\n");

        set_voltage_buffer[panel]     = strtol(set_voltage_str, &end, 10); // convert to value

        SerialUSB.print("Setting voltage buffer to ");
        SerialUSB.print(set_voltage_buffer[panel]);
        SerialUSB.print("\n");

        moveCursorRight();
    }
    else if (current_set_mode) {

        set_current_str[cursor_index]     = thischar[0];
        set_current_str[decimal_position] = ' '; // convert the decimal to a space so strol will parse it as two numbers

        char* end;
        int characteristic        = strtol(set_current_str, &end, 10);
        int mantissa              = strtol(end,             &end, 10);

        set_current_buffer[panel] = (10 * characteristic) + mantissa;

        SerialUSB.print("Setting current buffer to ");
        SerialUSB.print(set_current_buffer[panel]);
        SerialUSB.print("\n");

        moveCursorRight();
    }
}

void buildStatus (int panel)
{
    int x_offset=0;
    if (panel==1) x_offset=240;

    //------------------------------------------------------------------------------------------------------------------
    // Draw Background Boxes
    //------------------------------------------------------------------------------------------------------------------

    // over voltage
    CleO.RectangleColor(ovp_ok[panel] ? color_good : color_bad);
    CleO.RectangleXY(x_offset+200, 40, 80,80);

    // over current
    CleO.RectangleColor(ovc_ok[panel] ? color_good : color_bad);
    CleO.RectangleXY(x_offset+200, 120, 80,80);

    // on/off
    CleO.RectangleColor(output_on[panel] ? color_on : color_off);
    CleO.RectangleXY(x_offset+60, 280, 120,80);

    // status
    CleO.RectangleColor(status_ok[panel] ? color_good : color_bad);
    CleO.RectangleXY(x_offset+180, 280, 120,80);

    // voltage set
    CleO.RectangleColor(voltage_set_mode ? set_color : background_color);
    CleO.RectangleXY(x_offset+60, 200, 120,80);

    // current set
    CleO.RectangleColor(current_set_mode ? set_color : background_color);
    CleO.RectangleXY(x_offset+180, 200, 120,80);


    //------------------------------------------------------------------------------------------------------------------
    // Draw Divider Lines
    //------------------------------------------------------------------------------------------------------------------

    // Status Lines
    CleO.Line(0   + x_offset, 80  , 240 + x_offset, 80);  // A
    CleO.Line(0   + x_offset, 160 , 240 + x_offset, 160); // B
    CleO.Line(0   + x_offset, 240 , 240 + x_offset, 240); // C
    CleO.Line(0   + x_offset, 320 , 240 + x_offset, 320); // D
    CleO.Line(160 + x_offset, 0   , 160 + x_offset, 160); // E
    CleO.Line(120 + x_offset, 160 , 120 + x_offset, 320); // F


    //------------------------------------------------------------------------------------------------------------------
    // Update Text
    //------------------------------------------------------------------------------------------------------------------

    uint16_t deciVolts = countsToDeciVolts(read_voltage_counts_average[panel]);
    uint16_t microAmps = countsToMicroAmps(read_current_counts_average[panel]);

    //SerialUSB.println(mv);
    char buf_volts[10];
    char buf_current[10];

    sprintf(buf_volts,   "% 4lu.%01luV",  deciVolts / 10,    deciVolts % 10);
    sprintf(buf_current, "% 2lu.%02lumA", microAmps / 1000,  (microAmps % 1000)/100);

    CleO.LineWidth(1);
    CleO.StringExt(FONT_SANS_3 , 80  + x_offset , 40  , display_text_color , MM , 0 , 0 , buf_volts); // voltage

    CleO.StringExt(FONT_SANS_3 , 80  + x_offset , 120 , display_text_color , MM , 0 , 0 , buf_current); // current

    CleO.StringExt(FONT_SANS_3 , 200 + x_offset , 40  , display_text_color , MM , 0 , 0 , "OVP");  // Overvoltage
    CleO.StringExt(FONT_SANS_3 , 200 + x_offset , 120 , display_text_color , MM , 0 , 0 ,"OVC");  // Overcurrent



    char *output_str [2] = {(char*)"OFF", (char*)"ON"};
    CleO.StringExt(FONT_SANS_3 , 60  + x_offset , 280 , display_text_color , MM , 0 , 0 , output_str[output_on[panel]]); // On/Off

    CleO.StringExt(FONT_SANS_3 , 180 + x_offset , 280 , display_text_color , MM , 0 , 0 , "STAT"); // Status


    //------------------------------------------------------------------------------------------------------------------
    // Voltage & Current Setting
    //------------------------------------------------------------------------------------------------------------------

    updateSetStrings(panel);

    // Cursor highlighting in voltage/current set mode

    if (keypad_enabled[panel] && (voltage_set_mode || current_set_mode) ) {


        for (int i=0; i<(4+current_set_mode); i++) { // current_set_mode string is 1 character longer
            if (i==cursor_index) {
                set_voltage_highlight[i] = set_voltage_str[cursor_index];
                set_current_highlight[i] = set_current_str[cursor_index];
            }
            else {
                set_voltage_highlight[i] = ' ';
                if (i!=decimal_position) set_current_highlight[i] = ' ';
            }
        }

        if (voltage_set_mode) set_voltage_str[cursor_index] = ' ';
        if (current_set_mode) set_current_str[cursor_index] = ' ';


        if      (voltage_set_mode) CleO.StringExt(FONT_VGA_16 , 60  + x_offset , 200 , touch_color , MM , 0 , 0 , set_voltage_highlight); // voltage set
        else if (current_set_mode) CleO.StringExt(FONT_VGA_16 , 180 + x_offset , 200 , touch_color , MM , 0 , 0 , set_current_highlight); // current set
    }

    CleO.StringExt(FONT_VGA_16 , 60  + x_offset , 200 , display_text_color , MM , 0 , 0 , set_voltage_str); // voltage set
    CleO.StringExt(FONT_VGA_16 , 180 + x_offset , 200 , display_text_color , MM , 0 , 0 , set_current_str); // current set
}

void processLongPress() {
    for (int panel=0; panel<2; panel++) {
        if (current_tag==tag_powers[panel]) {
            output_on[panel] = !output_on[panel];
        }
        else if (current_tag==tag_voltages[panel] && !current_set_mode) // can't switch
        {
            SerialUSB.println("Toggling Voltage Set Mode");
            keypad_enabled[panel] = !keypad_enabled[panel];
            voltage_set_mode      = !voltage_set_mode;

            // update the set voltage when we leave set mode; keep the value in a buffer before that
            if (!voltage_set_mode) {

                // SOMEHOW the function max(x,y) doesn't work here... maybe because we are accessing an array ?
                set_voltage_buffer[panel] = (abs_max_voltage < set_voltage_buffer[panel]) ? abs_max_voltage : set_voltage_buffer[panel];

                // convert to deciVolts!  ;;; don't forget
                set_decivolts[panel]      = 10 * set_voltage_buffer[panel];

                SerialUSB.print("set_voltage_buffer = ");
                SerialUSB.print(set_voltage_buffer[panel]);
                SerialUSB.print("\n");

                SerialUSB.print("max function returned");
                SerialUSB.print( max(abs_max_voltage, set_voltage_buffer[panel]));
                SerialUSB.print("\n");

                SerialUSB.print("Max voltage = ");
                SerialUSB.print(abs_max_voltage);
                SerialUSB.print("\n");


                SerialUSB.print("Setting panel ");
                SerialUSB.print(panel);
                SerialUSB.print(" voltage to ");
                SerialUSB.print(set_decivolts[panel]);
                SerialUSB.print("\n");
            }

            cursor_index = 0;
        }
        else if (current_tag==tag_currents[panel] && !voltage_set_mode)
        {
            SerialUSB.println("Toggling Current Set Mode");
            keypad_enabled[panel] = !keypad_enabled[panel];
            current_set_mode      = !current_set_mode;
            cursor_index          = 0;

            // update the set current when we leave set mode; keep the value in a buffer before that
            if (!current_set_mode) {

                set_current_buffer[panel] = (abs_max_current < set_current_buffer[panel]) ? abs_max_current : set_current_buffer[panel];

                // convert to microamps!  don't forget
                set_microamps[panel]      = 100 * set_current_buffer[panel];

                SerialUSB.print("Max current = ");
                SerialUSB.print(abs_max_current);
                SerialUSB.print("\n");

                SerialUSB.print("Setting panel ");
                SerialUSB.print(panel);
                SerialUSB.print(" current to ");
                SerialUSB.print(set_microamps[panel]);
            }
        }
    }
}

void processShortPress() {
    switch (current_tag) {
        case tag_0     : setDigitValue(0x0, 0x1&keypad_enabled[1]) ; break ;
        case tag_1     : setDigitValue(0x1, 0x1&keypad_enabled[1]) ; break ;
        case tag_2     : setDigitValue(0x2, 0x1&keypad_enabled[1]) ; break ;
        case tag_3     : setDigitValue(0x3, 0x1&keypad_enabled[1]) ; break ;
        case tag_4     : setDigitValue(0x4, 0x1&keypad_enabled[1]) ; break ;
        case tag_5     : setDigitValue(0x5, 0x1&keypad_enabled[1]) ; break ;
        case tag_6     : setDigitValue(0x6, 0x1&keypad_enabled[1]) ; break ;
        case tag_7     : setDigitValue(0x7, 0x1&keypad_enabled[1]) ; break ;
        case tag_8     : setDigitValue(0x8, 0x1&keypad_enabled[1]) ; break ;
        case tag_9     : setDigitValue(0x9, 0x1&keypad_enabled[1]) ; break ;
        case tag_point : setDecimalPoint()                  ; break ;
        case tag_left  : moveCursorLeft()                   ; break ;
        case tag_right : moveCursorRight()                  ; break ;
    }
}

void buildKeypad (int keypad)
{

    int x_offset=0;
    if (keypad==0)
        x_offset = windowWidth;


    CleO.LineWidth(1);

    /* Draw a rectangle at (160, 240) point with 200 width, 400 height */


    //------------------------------------------------------------------------------------------------------------------
    // Background Color
    //------------------------------------------------------------------------------------------------------------------
    CleO.RectangleColor(keypad_color);
    CleO.RectangleXY(x_offset+windowWidth/2, 0+windowHeight/2, 240, 320);

    //------------------------------------------------------------------------------------------------------------------
    // Build each key
    //------------------------------------------------------------------------------------------------------------------

    buildKey(40   + x_offset , 40  , 80 , tag_7 , (char*)"7");
    buildKey(120  + x_offset , 40  , 80 , tag_8 , (char*)"8");
    buildKey(200  + x_offset , 40  , 80 , tag_9 , (char*)"9");
    buildKey(40   + x_offset , 120 , 80 , tag_4 , (char*)"4");
    buildKey(120  + x_offset , 120 , 80 , tag_5 , (char*)"5");
    buildKey(200  + x_offset , 120 , 80 , tag_6 , (char*)"6");
    buildKey(40   + x_offset , 200 , 80 , tag_1 , (char*)"1");
    buildKey(120  + x_offset , 200 , 80 , tag_2 , (char*)"2");
    buildKey(200  + x_offset , 200 , 80 , tag_3 , (char*)"3");
    buildKey(40   + x_offset , 280 , 80 , tag_0 , (char*)"0");
    buildKey(120  + x_offset , 280 , 80 , tag_point , (char*)".");

    buildKey(200  + x_offset , 260 , 80 , tag_left  , (char*)"l");
    buildKey(200  + x_offset , 300 , 80 , tag_right , (char*)"r");

    //------------------------------------------------------------------------------------------------------------------
    // Draw Dividing Lines
    //------------------------------------------------------------------------------------------------------------------

    // Status Lines
    CleO.Line(x_offset+80  , 0   , x_offset+80  , 320);
    CleO.Line(x_offset+160 , 0   , x_offset+160 , 320);
    CleO.Line(x_offset+0   , 80  , x_offset+240 , 80);
    CleO.Line(x_offset+0   , 160 , x_offset+240 , 160);
    CleO.Line(x_offset+0   , 240 , x_offset+240 , 240);

    CleO.Line(x_offset+160   , 280 , x_offset+240 , 280);
}

void buildKey    (int x, int y, int box_size, int tag, char* text)
{
    bool tagged = (current_tag==tag);

    int tag_size = box_size-(tagged ? 0 : bounding_margin);
    int x_size   = tag_size;
    int y_size   = (tag==tag_left||tag==tag_right) ? tag_size/2 : tag_size;

    CleO.Tag(tag);

    CleO.RectangleColor(tagged ? touch_color : keypad_color);
    CleO.RectangleXY(x, y, x_size, y_size);

    //-the dot is a special case----------------------------------------------------------------------------------------
    if (tag==tag_point) {
        int dot_size = 4;
        CleO.RectangleColor(keypad_text_color);
        CleO.RectangleXY(x, y, dot_size, dot_size);
    }
    //-the arrows are also special cases--------------------------------------------------------------------------------
    else if (tag==tag_left) {
        int needle_length = 25;
        int needle_radius = 0;
        CleO.NeedleWidth (10);

        CleO.NeedleExt(x+needle_length/4, y, needle_length, keypad_text_color, 90,  needle_radius);
    }
    //-the arrows are also special cases--------------------------------------------------------------------------------
    else if (tag==tag_right) {
        int needle_length = 25;
        int needle_radius = 0;
        CleO.NeedleWidth (10);

        CleO.NeedleExt(x-needle_length/4, y, needle_length, keypad_text_color, 270, needle_radius);
    }
    //-normal characters are normal-------------------------------------------------------------------------------------
    else {
        CleO.StringExt(FONT_SANS_4 , x,y, keypad_text_color , MM , 0 , 0 , text);
    }
}

void processButtons ()
{
    CleO.TouchCoordinates(x, y, dur, current_tag);

    // manual tagging for >14 --- the software is REALLY buggy otherwise
    if (current_tag > 13)  {
        for (int window=0; window<2; window++) {
            if (!keypad_enabled[!window] && y>160 && x<240+window*windowWidth) { // We DONT want to parse this screen as buttons if the other side is using it as a keypad

                //--------------------------------------------------------------------------------------------------
                // Voltage Set Mode
                //--------------------------------------------------------------------------------------------------

                //  top         left     right       bottom
                if (          y > 160+10
                        &&    y < 240-10
                        &&    x > window*windowWidth + 0+10
                        &&    x < window*windowWidth + 120-10
                   )
                {
                    current_tag = tag_voltages[window];
                }

                //--------------------------------------------------------------------------------------------------
                // Current Set Mode
                //--------------------------------------------------------------------------------------------------

                else if (
                           y > 160+10
                        && y < 240-10
                        && x > window*windowWidth+120 +10
                        && x < window*windowWidth+240-10
                        )
                {
                    current_tag = tag_currents[window];
                }

                //--------------------------------------------------------------------------------------------------
                // Power Toggle
                //--------------------------------------------------------------------------------------------------

                else if (
                           y > 240+10
                        && x >  window*windowWidth + 0  +10
                        && x <  window*windowWidth + 120-10
                        && y < 320-10
                        )
                {
                    current_tag = tag_powers[window];
                }
            }
        }
    }

    // press (1  , &short_high_cnt , &short_low_cnt , &short_press_allow , &short_press_detected);
    shortPress (&short_press_detected);
    press (15 , &long_high_cnt  , &long_low_cnt  , &long_press_allow  , &long_press_detected);

    last_dur = dur;
}

void updateScreen()
{
    //------------------------------------------------------------------------------------------------------------------
    // Start Drawing Screen
    //------------------------------------------------------------------------------------------------------------------

    CleO.Start();

    //------------------------------------------------------------------------------------------------------------------
    // Build Keypads / Status Screens
    //------------------------------------------------------------------------------------------------------------------

    CleO.RectangleJustification(MM);
    CleO.LineColor(line_color);

    if (keypad_enabled[0]) {
        buildStatus(0);
        buildKeypad(0);
    } else if (keypad_enabled[1]) {
        buildStatus(1);
        buildKeypad(1);
    } else {
        buildStatus(0);
        buildStatus(1);
    }

    CleO.LineWidth(3);
    CleO.Line(windowWidth, 0, windowWidth, windowHeight); // center line vertical

    //------------------------------------------------------------------------------------------------------------------
    // Collect Tags
    //------------------------------------------------------------------------------------------------------------------

    processButtons();

    //------------------------------------------------------------------------------------------------------------------
    // Parse Tags
    //------------------------------------------------------------------------------------------------------------------

    // long press for power and opening the keypad
    if (long_press_detected) {
        processLongPress();
    }

    // short presses are OK for keypad touches
    else if (short_press_detected && (keypad_enabled[0] || keypad_enabled[1])) {
        processShortPress();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Update Screen
    //------------------------------------------------------------------------------------------------------------------

    CleO.Show();
}
#endif /* SCREEN_H */
