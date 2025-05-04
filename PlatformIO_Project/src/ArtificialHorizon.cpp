#include "ArtificialHorizon.h"

ArtificialHorizon::ArtificialHorizon(TFT_eSPI &tft):
    _tft(tft),          // Reference to TFT instance
    _canvas(&tft)       // Initialize sprite with TFT instance
{}

void ArtificialHorizon::begin() {
    _canvas.createSprite(attitude_indicator_width_pixels, attitude_indicator_height_pixels);
}

void ArtificialHorizon::update(float roll_angle , float pitch_angle, float psi_angle) {
    _canvas.fillSprite(TFT_BLUE);
    drawGround(_canvas, pitch_angle, roll_angle);
    drawPitchScale(_canvas, -pitch_angle);
    drawCenterMarker(_canvas, TFT_YELLOW);
    drawBankAngleIndicator(_canvas, roll_angle, TFT_WHITE, 15);
    drawHeadingIndicator(_canvas, psi_angle);
}

void ArtificialHorizon::draw() {
    _canvas.pushSprite(0, 0);
}

// Your original functions (copy-paste exactly as you had them)
void ArtificialHorizon::drawPitchScale(TFT_eSprite &sprite, float pitchAngle, uint16_t color) {

    const int w = attitude_indicator_width_pixels;
    // Reduce height to leave some space for the heading indication
    const int h = attitude_indicator_height_pixels - heading_indication_height_pixels;
    const int centerX = w/2;
    const int centerY = attitude_indicator_height_pixels/2;     // Center Y position (horizon at 0° pitch)
    const int smallMarkLength = 30;                             // Length of 5° marks
    const int largeMarkLength = 50;                             // Length of 10° marks
    const int zeroMarkLength = 70;                              // Length of 0° mark
    const int extremeMarkLength = 40;                           // Shorter marks for extreme angles

    // Draw pitch scale from -90° to +90°
    for (int angle = -90; angle <= 90; angle += 5) {
        // Calculate Y position relative to current pitch
        int yPos = centerY - (pitchAngle - angle) * pixels_per_deg;
        
        // Only draw if within visible area
        if (yPos >= 0 && yPos < h) {
            bool isExtremeAngle = (abs(angle) > 30);
            int markLength = smallMarkLength;
            
            if (angle % 10 == 0) {
                // Major marks (10° intervals)
                markLength = isExtremeAngle ? extremeMarkLength : largeMarkLength;
                
                // Special handling for 0°
                if (angle == 0) {
                    markLength += 10; // Make center line longer
                    sprite.drawFastHLine(centerX - zeroMarkLength/2, yPos, zeroMarkLength, color); // Full width line
                    sprite.drawFastHLine(centerX - zeroMarkLength/2, yPos+1, zeroMarkLength, color); // Double line
                    continue;
                }
                
                // Add numeric labels for 10° intervals
                sprite.setTextDatum(MC_DATUM);
                //uint16_t bgColor = (yPos < centerY - pitchAngle * pixelsPerDegree) ? TFT_BLUE : TFT_BROWN;
                //sprite.setTextColor(color, bgColor);
                sprite.setTextColor(color);
                
                // Draw labels at standard positions (±10° to ±30°)
                if (abs(angle) <= 30) {
                    sprite.drawNumber(abs(angle), centerX - markLength/2 - 15, yPos, 2);
                    sprite.drawNumber(abs(angle), centerX + markLength/2 + 15, yPos, 2);
                }
                // Draw labels closer to center for extreme angles
                else {
                    String label = String(abs(angle));
                    int offset = isExtremeAngle ? 25 : 35;
                    sprite.drawString(label, centerX - offset, yPos, 2);
                    sprite.drawString(label, centerX + offset, yPos, 2);
                }
            }
            
            // Draw the mark line
            sprite.drawFastHLine(centerX - markLength/2, yPos, markLength, color);
            
            // Add small ticks between 5° marks for extreme angles
            if (isExtremeAngle && angle % 10 != 0) {
                sprite.drawFastHLine(centerX - 10, yPos, 20, color);
            }
        }
    }
  }

void ArtificialHorizon::drawCenterMarker(TFT_eSprite &sprite, uint16_t color, int horizontalArm, int verticalArm) {
    const int centerX = sprite.width()/2;
    const int centerY = sprite.height()/2;
    const int markerSize = 4;     // Size of central circle
    const int armWidth = 4;       // Thickness of L-shape arms
    const int spacing = 30;       // Space between center and L-shapes
  
    // Central circle
    sprite.fillCircle(centerX, centerY, markerSize, color);
  
    // Left L-shape
    // Horizontal arm (left-pointing)
    sprite.fillRect(centerX - spacing - horizontalArm, centerY - armWidth/2, 
    horizontalArm, armWidth, color);
    // Vertical arm (upward-pointing)
    sprite.fillRect(centerX - spacing - armWidth/2, centerY - verticalArm, 
    armWidth, verticalArm, color);
  
    // Right L-shape
    // Horizontal arm (right-pointing)
    sprite.fillRect(centerX + spacing, centerY - armWidth/2, 
    horizontalArm, armWidth, color);
    // Vertical arm (upward-pointing)
    sprite.fillRect(centerX + spacing - armWidth/2, centerY - verticalArm, 
    armWidth, verticalArm, color);
}

void ArtificialHorizon::drawBankAngleIndicator(TFT_eSprite &sprite, float bankAngle,  uint16_t color, int pointerSize) {
    const int centerX = sprite.width()/2;
    const int centerY = sprite.height()/2;
    const int radius = bank_indication_radius_pixels;   // Radius of the arc
    const int arcThickness = 2;                         // Thickness of the arc line
    const int tickLength = 8;                           // Length of tick marks
    const int startAngle = 90;                          // Start angle of arc
    const int endAngle = 270;                           // End angle of arc
  
    // Draw the fixed arc
    //sprite.drawSmoothArc(centerX, centerY, radius, radius-arcThickness, startAngle, endAngle, color, TFT_BLACK);
    sprite.drawArc(centerX, centerY, radius, radius-arcThickness, startAngle, endAngle, color, TFT_BLACK);
  
    // Draw tick marks (every 10 degrees)
    for (int angle = -90; angle <= 90; angle += 10) {
    float rad = angle * DEG_TO_RAD;
    int x1 = centerX + radius * sin(rad);
    int y1 = centerY - radius * cos(rad);
    int x2 = centerX + (radius - tickLength) * sin(rad);
    int y2 = centerY - (radius - tickLength) * cos(rad);
    sprite.drawLine(x1, y1, x2, y2, color);
  
    // Add labels at 30° intervals
    if (abs(angle) % 30 == 0 && angle != 0) {
    String label = String(abs(angle));
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextColor(color);
    sprite.drawString(label, 
      centerX + (radius - tickLength - 10) * sin(rad),
      centerY - (radius - tickLength - 10) * cos(rad),
      2);
    }
    }
  
    // Draw the movable pointer (triangle)
    float pointerRad = bankAngle * DEG_TO_RAD;
    if (pointerRad > PI/2){pointerRad = PI/2;}
    else if (pointerRad < -PI/2){pointerRad = -PI/2;}

    int pointerX = centerX + (radius - pointerSize) * sin(pointerRad);
    int pointerY = centerY - (radius - pointerSize) * cos(pointerRad);
  
    sprite.fillTriangle(
    pointerX, pointerY,
    pointerX + pointerSize * sin(pointerRad + 0.8), 
    pointerY - pointerSize * cos(pointerRad + 0.8),
    pointerX + pointerSize * sin(pointerRad - 0.8), 
    pointerY - pointerSize * cos(pointerRad - 0.8),
    color);
}

void ArtificialHorizon::drawGround(TFT_eSprite &sprite, float pitchAngle, float rollAngle, uint16_t groundColor) {
    const int w = attitude_indicator_width_pixels;
    const int h = attitude_indicator_height_pixels;
  
    // Convert angles to radians
    float pitchRad = pitchAngle * DEG_TO_RAD;
    float rollRad = rollAngle * DEG_TO_RAD;
    
    // Horizon line slope
    float c_tanRoll = tan(-rollRad);
    
    // Horizon line offset
    float b_offset = pixels_per_deg * -pitchAngle - h/2 - c_tanRoll*w/2;
  
    // Define interception with all 4 edges of the rectangular screen
    // 1 --> top
    // 2 --> right
    // 3 --> bottom
    // 4 --> left
    float intersection_xy[4][2] = {};
  
    // Top
    intersection_xy[0][0] = -b_offset/c_tanRoll;
    intersection_xy[0][1] = 0;
  
    // Right
    intersection_xy[1][0] = w;
    intersection_xy[1][1] = c_tanRoll*w + b_offset;
  
    // Bottom
    intersection_xy[2][0] = (-b_offset - h)/c_tanRoll;
    intersection_xy[2][1] = -h;
  
    // Left
    intersection_xy[3][0] = 0;
    intersection_xy[3][1] = b_offset;
  
    // Get the two intersections with the rectangle
    float intersection_rect_xy[2][2] = {};
    int intersection_edge[2] = {};
    int counter = 0;

    if ((intersection_xy[0][0] > 0) && (intersection_xy[0][0]) < w) {
        intersection_rect_xy[counter][0] = intersection_xy[0][0];
        intersection_rect_xy[counter][1] = intersection_xy[0][1];
        intersection_edge[counter] = 0;
        counter++;
    }

    if ((intersection_xy[1][1] < 0) && (intersection_xy[1][1]) > -h) {
        intersection_rect_xy[counter][0] = intersection_xy[1][0];
        intersection_rect_xy[counter][1] = intersection_xy[1][1];
        intersection_edge[counter] = 1;
        counter++;
    }

    if ((counter <= 1) && (intersection_xy[2][0] > 0) && (intersection_xy[2][0]) < w) {
        intersection_rect_xy[counter][0] = intersection_xy[2][0];
        intersection_rect_xy[counter][1] = intersection_xy[2][1];
        intersection_edge[counter] = 2;
        counter++;
    }

    if ((counter <= 1) && (intersection_xy[3][1] < 0) && (intersection_xy[3][1]) > -h) {
        intersection_rect_xy[counter][0] = intersection_xy[3][0];
        intersection_rect_xy[counter][1] = intersection_xy[3][1];
        intersection_edge[counter] = 3;
    }
  
    // Check if there is interception
    bool intecept_flg = true;
        if ((intersection_edge[1] == 0)){
            intecept_flg = false;
        }
    
    // index of P1 and P2
    // P1 ---> point intercept with smaller X value
    // P2 ---> point intercept with greater X value
    int idx_p1 = 0;
    int idx_p2 = 1;

    // Check if vehicle is upside down
    int edge_search_increment = 1;
    if (abs(rollRad) > PI/2){
        edge_search_increment = -1;
    }

    // If there is interception, find the other edges and plot
    if (intecept_flg){
        
        // If first intercept is greater, assign it to P2 and vice-versa
        if (intersection_rect_xy[0][0] > intersection_rect_xy[1][0]){
            idx_p1 = 1;
            idx_p2 = 0;
        }

        // Find next edge to move along after P2
        for (int i = 1; i < 4; i++)
        {
            // Next edge index
            int idx_next = (intersection_edge[idx_p2] + edge_search_increment + 4) % 4;

            // Initialize vertex vector
            float vertex[2] = {};

            // Depending on the edge, compute the vertex
            if ( (intersection_edge[idx_p2] + idx_next) == 1 ){
                vertex[0] = w;
                vertex[1] = 0;
            }else if (((intersection_edge[idx_p2] + idx_next) == 3) && (idx_next == 1 || idx_next == 2))
            {
                vertex[0] = w;
                vertex[1] = -h;
            }else if ( (intersection_edge[idx_p2] + idx_next) == 5 )
            {
                vertex[0] = 0;
                vertex[1] = -h;
            }else
            {
                vertex[0] = 0;
                vertex[1] = 0;
            }
            
            // Draw triangle
            sprite.fillTriangle(intersection_rect_xy[idx_p1][0], -intersection_rect_xy[idx_p1][1],
                                intersection_rect_xy[idx_p2][0], -intersection_rect_xy[idx_p2][1],
                                vertex[0],                       -vertex[1],
                                groundColor
            );

            // Check if next vertex is point P1
            // If so, stop the while loop
            if (idx_next == intersection_edge[idx_p1]){
                break;
            }

            // Update value of P2
            intersection_rect_xy[idx_p2][0] = vertex[0];
            intersection_rect_xy[idx_p2][1] = vertex[1];
            intersection_edge[idx_p2] = idx_next;
        }

    } else {
        // There was no intersection on the screen
        // Determine if the screen should be colored brown or blue

        // If the right edge intercept is positive, and vehicle is not upside down, draw the ground
        if (intersection_xy[1][1] > 0 && (abs(rollRad) < PI/2)){
            sprite.fillRect(0,0,w,h,groundColor);
        }
    }
}

void ArtificialHorizon::drawHeadingIndicator(TFT_eSprite &sprite, float heading, uint16_t color) {
    const int centerX = attitude_indicator_width_pixels / 2;
    const int bottomY = attitude_indicator_height_pixels - 2;   // Position at bottom
    const int smallMarkLength = 6;
    const int largeMarkLength = 12;
    
    // Draw baseline
    sprite.drawFastHLine(0, bottomY, sprite.width(), color);
    
    // Draw marks and labels
    for (int angle = -120; angle < 360; angle += 10) {
        // Calculate screen position (rotated by current heading)
        float relativeAngle = angle - heading;
        int xPos = centerX + (pixels_per_deg_heading * relativeAngle);
        
        // Only draw if visible on screen
        if (xPos >= 0 && xPos < sprite.width()) {
            // Determine mark size
            int markLength = ((angle+360) % 30 == 0) ? largeMarkLength : smallMarkLength;
            
            // Draw mark
            sprite.drawFastVLine(xPos, bottomY - markLength, markLength, color);
            
            // Draw cardinal directions
            if ((angle + 360) % 90 == 0) {

                const char* direction = "";
                if ((angle+360) % 360 == 0){
                    direction = "N";
                }
                else if ((angle+360) % 360 == 90)
                {
                    direction = "E";
                }else if ((angle+360) % 360 == 180)
                {
                    direction = "S";
                }else{
                    direction = "W";
                }
                
                sprite.setTextDatum(BC_DATUM); // Bottom-center alignment
                sprite.drawString(direction, xPos, bottomY - markLength - 2, 2);
            }
            
            // Draw numeric labels at 30° intervals
            else if (angle % 30 == 0) {
                sprite.setTextDatum(BC_DATUM);
                if (angle < 0){
                    sprite.drawNumber(angle + 360, xPos, bottomY - markLength - 2, 2);
                } else if (angle > 360)
                {
                    sprite.drawNumber(angle - 360, xPos, bottomY - markLength - 2, 2);
                }else{
                    sprite.drawNumber(angle, xPos, bottomY - markLength - 2, 2);
                }
            }
        }
    }

    // Add text with current heading
    sprite.setTextColor(color, TFT_BLACK); // Black background
    sprite.drawNumber(heading, centerX, bottomY, 2);
}