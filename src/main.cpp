#include <raylib.h>
#include <ctime>
#include <iostream>
#include <raymath.h>
#include <vector>
#include <deque>
#include <string>
using namespace std;
const double UniverseGravConst = 6.674e-11;

class Knob {
    double radius;
    Color color;
    Vector2 position;
    bool sliderpressed;

    public:
    void DrawKnob() {
        DrawCircleV(position, radius, color);
    }
    void DrawKnobPositionX(Font font, Vector2 baseposition, float offset) {
        string knobpos = to_string(GetKnobValue(baseposition).x);
        DrawTextEx(font, knobpos.c_str(), {GetPosition().x + offset + MeasureTextEx(font, knobpos.c_str(), radius * 0.5, 1).x, GetPosition().y}, radius, 1, color);
    }
    void DrawKnobPositionY(Font font, Vector2 baseposition, float offset) {
        string knobpos = to_string(GetKnobValue(baseposition).y);
        DrawTextEx(font, knobpos.c_str(), {GetPosition().x , GetPosition().y + offset + MeasureTextEx(font, knobpos.c_str(), radius * 0.5, 1).y}, radius, 1, color);
    }
    void SetPosition(float newposx, float newposy) {
        position.x = newposx;
        position.y = newposy;
    }
    void SetColor(Color newcolor) {
        color = newcolor;
    }
    void SetRadius(double newradius) {
        radius = newradius;
    }
    Vector2 GetPosition() {
        return position;
    }
    Vector2 GetKnobValue(Vector2 baseposition) {
        return Vector2Subtract(position, baseposition);
    }
    void SetKnobXValue(float newknobpos) {
        position.x = newknobpos;
    }
    void SetKnobYValue(float newknobpos) {
        position.y = newknobpos;
    }
    void UpdateKnobX(Camera2D camera) {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointCircle(GetScreenToWorld2D(GetMousePosition(), camera), position, radius)){
            sliderpressed = true;
        }else if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) && sliderpressed) {
            sliderpressed = false;
        }

        if(sliderpressed) {
            position.x = GetScreenToWorld2D(GetMousePosition(), camera).x;
        }
    }
    void UpdateKnobY(Camera2D camera) {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointCircle(GetScreenToWorld2D(GetMousePosition(), camera), position, radius)){
            sliderpressed = true;
        }else if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) && sliderpressed) {
            sliderpressed = false;
        }

        if(sliderpressed) {
            position.y = GetScreenToWorld2D(GetMousePosition(), camera).y;
        }
    }
    bool GetSliderPressed(){
        return sliderpressed;
    }
    
    Knob(Vector2 p, double r, Color c) {
        radius = 5;
        color = c;
        position = p;
        sliderpressed = false;

    }
};

class Circle {
   double radius;
   Vector2 position;
   Color color;
   Vector2 vel;
   double mass;
   deque<Vector2> points;
   Vector2 force;
   bool selected;
   Knob knobx = {{0 ,0}, 0, BLANK};
   Knob knoby= {{0 ,0}, 0, BLANK};;

   public:
   void DrawMyCircle() {
       DrawCircle(position.x, position.y, radius, color);
       if(selected){
           knobx.DrawKnob();
           knoby.DrawKnob();
       }
   }
   void SetVel(Vector2 nums){
       vel = nums;
   }
   void ClearPoints(){
       points.clear();
   }
   Vector2 GetVel(){
       return vel;
   }
   Vector2 GetPosition(){
       return position;
   }
   double GetRadius(){
       return radius;
   }
   void SetSelect(bool tf){
       selected = tf;
   }
   void SetRadius(double newRadius){
       radius = newRadius;
   }
   void AddRadius(double newRadius){
       radius += newRadius;
   }
   void SetColor(Color newColor){
       color = newColor;
   }
   void SetPosition(Vector2 newpos){
       position = newpos;
   }
   bool GetSelect() {
       return selected;
   }
   bool GetKnobPressed() {
       return knobx.GetSliderPressed() || knoby.GetSliderPressed();
   }
   void SetForce(Vector2 f) {
       force = f;
   }
   void UpdatePosition(double DT) {
       position.x += vel.x * DT;
       position.y += vel.y * DT;
       points.push_back(position);
       if(points.size() == 2000){
           points.pop_front();
       }
   }
   void UpdatePlanetKnobs(Camera2D camera, Font font){
       knobx.UpdateKnobX(camera);
       knoby.UpdateKnobY(camera);
       knobx.DrawKnobPositionX(font, position, radius);
       knoby.DrawKnobPositionY(font, position, radius);
   }
   void UpDateKnobPos() {
       knobx.SetPosition(position.x + vel.x, position.y);
       knoby.SetPosition(position.x, position.y + vel.y);
   }
   void DrawVel(){
       DrawLine(position.x, position.y, position.x + vel.x, position.y, GREEN);
       DrawLine(position.x, position.y, position.x, position.y + vel.y, YELLOW);
       DrawLine(position.x, position.y, position.x + vel.x, position.y + vel.y, ORANGE);
   }
   void DrawAccel() {
       Vector2 acceleration = Vector2Scale(force, 1.0/mass);
       DrawLine(position.x, position.y, position.x + acceleration.x, position.y + acceleration.y, RAYWHITE);
   }
   void DrawSelected() {
       DrawCircleLines(position.x, position.y, radius * 1.25, PINK);
   }
   void CalculateForces(Circle circle){
       double distance = Vector2Distance(position, circle.position);
       double f = UniverseGravConst * (mass * circle.mass) / (distance * distance);
       Vector2 gravForce = Vector2Scale(Vector2Normalize(Vector2Subtract(circle.position, position)), f);
   
       force = Vector2Add(force, gravForce);
   }
   void UpdateVel(double DT) {
       Vector2 acceleration = Vector2Scale(force, 1.0/mass);
       vel.x += (acceleration.x * DT);
       vel.y += (acceleration.y * DT);
       UpDateKnobPos();
   } 
   void UpdateKnobVel(){
       vel.x = knobx.GetKnobValue(position).x;
       vel.y = knoby.GetKnobValue(position).y;
   }
   void DrawPoints() {
       for(int i = 0; i < points.size(); i++) {
           DrawPixelV(points[i], color);
       }
   }
   void SnapCameraTo(Camera2D *c) {
       c->target = position; 
   }
   Circle(Vector2 p, double r, double m, Color c){
       position = p;
       radius = r;
       color = c;
       mass = m;
       points = {};
       force = {0, 0};
       selected = false;
       knobx = {{position.x, position.y}, radius, GRAY};
       knoby = {{position.x, position.y}, radius, DARKGRAY};
       vel.x = knobx.GetKnobValue(position).x;
       vel.y = knoby.GetKnobValue(position).y;
       
   }
};
class Button {
    Vector2 position;
    Vector2 size;
    Color color;
    Color currentColor;
    string text;
    string updateText;
    string currentText;
    bool buttonState;
    bool uiSelect;
    int fontSize;
    Font font;
    Vector2 textSize;
    
    public:
    void DrawButton(){
        DrawRectangleV(position, size, currentColor);
        DrawTextEx(font, currentText.c_str(), {position.x + (size.x - textSize.x)/2 , position.y + (size.y - textSize.y)/2}, fontSize, 1.0, LIGHTGRAY);
    }
    bool UpdateButton() {
        if(CheckCollisionPointRec(GetMousePosition(), ButtonToRect())){
            SetColor({ 100, 120, 80, 255 });
            uiSelect = true;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !buttonState){
                buttonState = true;
                SetText(updateText);
            }
            else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && buttonState) {
                buttonState = false;
                SetText(text);
            }
        }else {
            SetColor(color);
            uiSelect = false;
        }
        return uiSelect;
    }
    bool IsButtonPressed () {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), ButtonToRect())){
            return true;
        } else {
            return false;
        }
    }
    bool GetButtonState(){
        return buttonState;
    }
    Vector2 GetButtonSize(){
        return size;
    }
    void SetButtonState(bool state) {
        buttonState = state;
    }
    void SetText(string t) {
        currentText = t;
        textSize = MeasureTextEx(font, t.c_str(), 40, 1);
    }
    void SetColor(Color c){
        currentColor = c;
    }
    Rectangle ButtonToRect() {
        float positionx = position.x;
        float positiony = position.y;
        float sizex = size.x;
        float sizey = size.y;
        Rectangle button = { positionx, positiony, sizex, sizey };
        return button;
    }
    Button(Vector2 p, Vector2 s, string t, int fS, Color c, Font f, string nT){
        position = p;
        color = c;
        currentColor = c;
        text = t;
        updateText = nT;
        buttonState = false;
        uiSelect = false;
        fontSize = fS;
        font = f;
        textSize = MeasureTextEx(f, text.c_str(), 40, 1);
        size = s;
        currentText = t; 
    }
    
};

class Slider {
    Vector2 bodyPosition;
    Vector2 linePosition;
    Vector2 bodySize;
    Vector2 lineStart;
    Vector2 lineEnd;
    Vector2 circlePosition;
    float circleRadius;
    float lineThickness;
    Color bodyColor;
    Color lineColor;
    Color circleColor;
    bool sliderPressed;
    bool uiSelect;
    
    public:
    void DrawSlider() {
        DrawRectangleV(bodyPosition, bodySize, bodyColor);
        DrawLineEx(lineStart, lineEnd, lineThickness, lineColor);
        DrawCircleV(circlePosition, circleRadius, circleColor); 
    }
    bool UpdateCircle(Vector2 newPosition) {
        if(CheckCollisionPointCircle(GetMousePosition(), circlePosition, circleRadius) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            sliderPressed = true;
            uiSelect = true;
        }else if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) && sliderPressed) {
            sliderPressed = false;
            uiSelect = false;
        }
        
        if(sliderPressed) {
            circlePosition.x = newPosition.x;
            if(circlePosition.x >= lineEnd.x) circlePosition.x = lineEnd.x;
            else if(circlePosition.x <= lineStart.x) circlePosition.x =lineStart.x;
        }
        return uiSelect;
    }
    Vector2 GetCirclePosition(){
        return circlePosition;
    }
    float GetCircleRadius() {
        return circleRadius;
    }
    Slider(Vector2 bP, Vector2 bS, Color bC, Color lC, Color cC){
        bodyPosition = bP;
        linePosition = {bodyPosition.x/2, bodyPosition.y/2};
        bodySize = bS;
        lineStart = {bodyPosition.x + 10, bodyPosition.y + bodySize.y/2};
        lineEnd = {bodyPosition.x + bodySize.x - 10, bodyPosition.y + bodySize.y/2};
        circlePosition = {bodyPosition.x + bodySize.x/2, bodyPosition.y + bodySize.y/2};
        circleRadius = 15;
        lineThickness = 3;
        bodyColor = bC;
        lineColor = lC;
        circleColor = cC;   
        sliderPressed = false;
        uiSelect = false;
    }
};
int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;
    bool isPlanetSelected = false;
    bool isUiSelected = false;
    bool isKnobSelected = false;
    bool canScrolltoZoom = true;
    vector<Circle> Planets = { {{(screenWidth/2), (screenHeight/2)}, 20, 1.11e16, SKYBLUE},
                               {{(screenWidth/2 - 240), screenHeight/2}, 10, 1e14, RED},
                               {{(screenWidth/2 - 260), screenHeight/2}, 3, 1e5, PURPLE},
                               {{(screenWidth/2 - 350), screenHeight/2}, 5, 1e2, YELLOW}}; 
    vector<Circle> DeletedPlanets = {};
    Circle NewPlanet = {{0, 0}, 15, 1, WHITE};

    InitWindow(screenWidth, screenHeight, "Gravity");

    
    Font defaultFont = GetFontDefault();
    Button PausePlay({screenWidth - 55, 5}, {50, 50}, ">", 40, DARKGRAY, defaultFont, "| |");
    Button AddPlanet({screenWidth - 55,  60}, {50, 50}, "+", 40, DARKGRAY, defaultFont, "+");
    Button Reset({5, 20}, {50, 50 }, "R", 40, DARKGRAY, defaultFont, "R");
    Button Preset({5, 75}, {50, 50 }, "P", 40, DARKGRAY, defaultFont, "P");
    Button Delete({screenWidth - 55, 115}, {50, 50}, "-", 40, DARKGRAY, defaultFont, "-");
    //Slider S1({screenWidth/4, 5}, {200, 35}, LIGHTGRAY, DARKGRAY, RED);
    
    Camera2D camera = { 0 };
    camera.target = Planets[0].GetPosition();
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;

    vector<Vector2> PlanetStartPosition = {};
    vector<Vector2> PlanetStartVel = {};

    for(int i = 0; i < Planets.size(); i++){
        PlanetStartPosition.push_back(Planets[i].GetPosition());
        PlanetStartVel.push_back(Planets[i].GetVel());
    }

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        string MousePos = "mouse x: " + to_string(GetMouseX()) + " mouse y: " + to_string(GetMouseY());
        DrawFPS(0, 0); 
        DrawTextEx(defaultFont, MousePos.c_str(), {100, 0}, 20, 1, DARKGREEN);
        float deltaTime = GetFrameTime(); 
        isUiSelected = false; 
        isPlanetSelected = false;
        isKnobSelected = false;
        
        BeginDrawing();
        ClearBackground(BLACK);

        isUiSelected |= PausePlay.UpdateButton();
        isUiSelected |= AddPlanet.UpdateButton();
        isUiSelected |= Reset.UpdateButton();
        isUiSelected |= Delete.UpdateButton();
        isUiSelected |= Preset.UpdateButton();
        //isUiSelected |= S1.UpdateCircle(GetMousePosition());
        
        if(PausePlay.GetButtonState()){
            for(int i = 0; i < Planets.size(); i++){
                Planets[i].SetForce({0, 0});
                for(int j = 0; j < Planets.size(); j++){
                    if(i != j){
                        Planets[i].CalculateForces(Planets[j]);
                    }
                }
            }
        }


        if(Reset.IsButtonPressed()){
             for(int i = 0; i < Planets.size() + DeletedPlanets.size(); i++){
                Planets[i].SetPosition(PlanetStartPosition[i]);
                Planets[i].SetVel(PlanetStartVel[i]);
                Planets[i].UpDateKnobPos(); 
                Planets[i].ClearPoints();
                camera.target = Planets[0].GetPosition();
            }
        }
        /*if(Preset.IsButtonPressed()){
            Planets.clear();
            Planets.push_back({{(screenWidth/2), (screenHeight/2)}, 20, 1.11e16, SKYBLUE});
            Planets.push_back({{(screenWidth/2 - 240), screenHeight/2}, 10, 1e14, RED});
            Planets.push_back({{(screenWidth/2 - 260), screenHeight/2}, 3, 1e5, PURPLE});
            Planets.push_back({{(screenWidth/2 - 350), screenHeight/2}, 5, 1e2, YELLOW});
        }*/
        
        BeginMode2D(camera);

            if(AddPlanet.GetButtonState() && !PausePlay.GetButtonState()){
                canScrolltoZoom = false;
                NewPlanet.SetPosition(GetScreenToWorld2D(GetMousePosition(), camera));
                NewPlanet.AddRadius(GetMouseWheelMove());
                NewPlanet.DrawMyCircle();
                if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
                    Planets.push_back(NewPlanet);
                    PlanetStartVel.push_back({0, 0});
                    PlanetStartPosition.push_back(NewPlanet.GetPosition());
                    AddPlanet.SetButtonState(false);
                    NewPlanet = {{0, 0}, 15, 1 , WHITE};
                    Planets[Planets.size()].UpdatePlanetKnobs(camera, defaultFont);
                }

            } else {
                canScrolltoZoom = true;
            }

            if (canScrolltoZoom){ 
                camera.zoom += GetMouseWheelMove()/20;
                if(camera.zoom <= 0.01) camera.zoom = 0.01;
            }

        for(int i = 0; i < Planets.size(); i++){
            
            Planets[i].DrawPoints();

            if(PausePlay.GetButtonState()){
                Planets[i].UpdateVel(deltaTime);
                Planets[i].UpdatePosition(deltaTime);
            }

            if(Planets[i].GetSelect()) {
                camera.target = Planets[i].GetPosition();
                if(IsKeyPressed(KEY_ONE)) Planets[i].SetColor(RED);
                else if(IsKeyPressed(KEY_TWO)) Planets[i].SetColor(ORANGE);
                else if(IsKeyPressed(KEY_THREE)) Planets[i].SetColor(YELLOW);
                else if(IsKeyPressed(KEY_FOUR)) Planets[i].SetColor(GREEN);
                else if(IsKeyPressed(KEY_FIVE)) Planets[i].SetColor(DARKGREEN);
                else if(IsKeyPressed(KEY_SIX)) Planets[i].SetColor(SKYBLUE);
                else if(IsKeyPressed(KEY_SEVEN)) Planets[i].SetColor(PURPLE);
                else if(IsKeyPressed(KEY_EIGHT)) Planets[i].SetColor(MAGENTA);
                else if(IsKeyPressed(KEY_NINE)) Planets[i].SetColor(WHITE);
                else if(IsKeyPressed(KEY_ZERO)) Planets[i].SetColor(BEIGE);
                
                Planets[i].DrawVel();
                Planets[i].DrawAccel();
                Planets[i].DrawSelected();

                if(!PausePlay.GetButtonState()){
                    Planets[i].UpdatePlanetKnobs(camera, defaultFont);
                    Planets[i].UpdateKnobVel();
                    if(Delete.IsButtonPressed()){
                        DeletedPlanets.push_back(Planets[i]);
                        Planets.erase(Planets.begin() + i);
                        PlanetStartPosition.erase(PlanetStartPosition.begin() + i);
                        PlanetStartVel.erase(PlanetStartVel.begin() + i);
                    }
                }
            }
            Planets[i].DrawMyCircle();
        }
        for(int i = 0; i < Planets.size(); i++){
            isKnobSelected |= Planets[i].GetKnobPressed();
        }
        for(int i = 0; i < Planets.size(); i++){
            isPlanetSelected |= Planets[i].GetSelect();
        }
        for(int i = 0; i < Planets.size(); i++){
            if(!isKnobSelected){
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointCircle(GetScreenToWorld2D(GetMousePosition(), camera), Planets[i].GetPosition(), Planets[i].GetRadius())){
                    for(int j = 0; j < Planets.size(); j++){
                        if(i != j) {
                            Planets[j].SetSelect(false);
                        }
                    }
                    Planets[i].SetSelect(true);
                }else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                    Planets[i].SetSelect(false);
                }
            }
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !isPlanetSelected && !isUiSelected){
            
            camera.target = Vector2Subtract(camera.target, Vector2Scale(GetMouseDelta(), 1/camera.zoom));
        }
    
        EndMode2D();

        PausePlay.DrawButton();
        AddPlanet.DrawButton();
        Reset.DrawButton(); 
        Delete.DrawButton();  
        Preset.DrawButton();          
        //S1.DrawSlider();
        
        EndDrawing();
    }
    CloseWindow();

    return 0;
}