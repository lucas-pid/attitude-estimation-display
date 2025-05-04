#include <TFT_eSPI.h>

class ArtificialHorizon {
public:
    // Constructor (requires TFT instance)
    ArtificialHorizon(TFT_eSPI &tft);
    
    // Initialization
    void begin();
    
    // Main functions
    void update(float roll_angle , float pitch_angle, float psi_angle);
    void draw();
    
    // Drawing functions
    static void drawGround(TFT_eSprite &sprite, float pitchAngle, float rollAngle, uint16_t groundColor = TFT_BROWN);
    static void drawPitchScale(TFT_eSprite &sprite, float pitchAngle, uint16_t color = TFT_WHITE);
    static void drawCenterMarker(TFT_eSprite &sprite, uint16_t color = TFT_WHITE, int horizontalArm = 25, int verticalArm = 25);
    static void drawBankAngleIndicator(TFT_eSprite &sprite, float bankAngle, uint16_t color = TFT_WHITE, int pointerSize = 10);
    static void drawHeadingIndicator(TFT_eSprite &sprite, float heading, uint16_t color = TFT_WHITE);

private:
    TFT_eSPI &_tft;
    TFT_eSprite _canvas;

    // Width and height of the attitude indicator sprite (in pixels)
    static const int attitude_indicator_width_pixels = 240;
    static const int attitude_indicator_height_pixels = 230;
    
    // Pixels per degree of pitch
    static const int pixels_per_deg = 5;

    // Bank angle indication radius (in pixels)
    static const int bank_indication_radius_pixels = 110;

    // Height of the heading indication (in pixels)
    static const int heading_indication_height_pixels = 30;
    
    // Pixels per degree of heading
    static const int pixels_per_deg_heading = 3;

};