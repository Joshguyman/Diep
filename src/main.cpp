#include "../include/raylib.h"
#include <ctime>
#include <iostream>
#include "../include/raymath.h"
#include "raylib.h"
#include <vector>
#include <deque>
#include <string>
using namespace std;

struct Entity {
    Vector2 pos;
    Vector2 vel;
};
struct Point {
    Entity point;
    int health;
    bool ishit;
};

class Tank {
    Entity tank;
    Vector2 gun;
    float maxspeed;
    float speed;
    float radius;
    int health;
    int tag;

    public:
    void DrawTank(Camera2D camera) {
        Vector2 worldtankpos = GetScreenToWorld2D(tank.pos, camera);
        Vector2 worldmousepos = GetScreenToWorld2D(GetMousePosition(), camera);
        Vector2 distance = Vector2Normalize(Vector2Subtract(worldmousepos, worldtankpos));
        gun = Vector2Add(worldtankpos, Vector2Scale(distance, radius * 1.75));
        DrawLineEx(worldtankpos, gun, radius/1.5, LIGHTGRAY);
        DrawCircleV(tank.pos, radius, SKYBLUE); 
    }
    void AddSpeed(float add, int maxspeed){
        speed += add;
        if(speed > maxspeed) {
            speed = maxspeed; 
        }
    }
    void SubSpeed(float sub){
        speed -= sub;
        if(speed < 0){
            speed = 0;
        }
    }
    void MoveTank(float dt) {
        bool didmove = false;
        tank.vel = {0, 0};
        if(IsKeyDown(KEY_W)){
           tank.vel = Vector2Add(tank.vel, {0, -1}); 
           didmove = true;
        }
        if(IsKeyDown(KEY_S)){
            tank.vel = Vector2Add(tank.vel, {0, 1});
            didmove = true;
        }
        if(IsKeyDown(KEY_D)){
            tank.vel = Vector2Add(tank.vel, {1, 0}); 
            didmove = true;
        }
        if(IsKeyDown(KEY_A)){
            tank.vel = Vector2Add(tank.vel, {-1, 0});
            didmove = true;
        }
        if(didmove){
            AddSpeed(maxspeed/.3 * dt, maxspeed);
        } else {
            SubSpeed(maxspeed * dt);
        }
        tank.pos = Vector2Add(tank.pos, Vector2Scale(Vector2Normalize(tank.vel), speed * dt)); 
    }
    Vector2 GetPos(){
        return tank.pos;
    }
    float GetRadius(){
        return radius;
    }
    Vector2 GetGunPos() {
        return gun;
    }
    Tank(Vector2 pos, int t) {
        tank.pos = pos;
        tank.vel = {0, 0};
        maxspeed = 300;
        speed = 0;
        radius = 20;
        health = 10;
        tag = t;
    } 
};
class Bullet {
    Entity bullet;
    float radius;
    int tag;
    float speed;
    
    public:
    float GetSpeed() {
        return speed;
    }
    void DrawBullet() {
        DrawCircleV(bullet.pos, radius, RED);
    }
    void MoveBullet(float dt) {
        bullet.pos = Vector2Add(bullet.pos, Vector2Scale(bullet.vel, speed * dt));
    }
    Vector2 GetPos() {
        return bullet.pos;
    }
    float GetRadius() {
        return radius;
    }
    Bullet(Vector2 pos, Vector2 vel, int t) {
        bullet.pos = pos;
        bullet.vel = vel;
        radius = 10;
        tag = t;
        speed = 250;
    }
};
class CirclePoint {
    Point circle;
    float radius;

    public:
    void DrawPoint(){
        if(circle.ishit) {
            DrawCircleV(circle.point.pos, radius, MAGENTA); 
        } else if(!circle.ishit){
            DrawCircleV(circle.point.pos, radius, PURPLE); 
        }
    }
    void SetHit(bool newstate) {
        circle.ishit = newstate;
    }
    Vector2 GetPos() {
        return circle.point.pos;
    }
    float GetRadius() {
        return radius;
    }

    CirclePoint(Vector2 pos) {
       circle.point.pos = pos;
       circle.point.vel = {0, 0};
       circle.health = 10;
       circle.health = false;
       radius =  15;
    }
};

int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;
    vector<Bullet> Bullets; 
    Tank tank({screenWidth/2, screenHeight/2}, 1); 
    CirclePoint c({screenWidth/4, screenHeight/2}); 
    Camera2D camera = {0};
    camera.target = tank.GetPos();
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    InitWindow(screenWidth, screenHeight, "Gravity");

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        float deltatime = GetFrameTime();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            Bullets.push_back(Bullet(tank.GetGunPos(), Vector2Normalize(Vector2Subtract(GetMousePosition(), tank.GetPos())), 1));
        }

        
        
        for(int i = Bullets.size() - 1 ; i >= 0; i--){
            if((Bullets[i].GetPos().x > screenWidth || Bullets[i].GetPos().x < 0) || (Bullets[i].GetPos().y > screenHeight || Bullets[i].GetPos().y < 0)){
                Bullets.erase(Bullets.begin() + i);

                if(CheckCollisionCircles(Bullets[i].GetPos(), Bullets[i].GetRadius(), c.GetPos(), c.GetRadius())){
                    c.SetHit(true);
                } else if(!CheckCollisionCircles(Bullets[i].GetPos(), Bullets[i].GetRadius(), c.GetPos(), c.GetRadius())){
                    c.SetHit(false);
                }
                continue;
            }
            
            Bullets[i].MoveBullet(deltatime);
            Bullets[i].DrawBullet();
        }
        c.DrawPoint();
        tank.MoveTank(deltatime);
        tank.DrawTank(camera);

        EndMode2D();
        EndDrawing();
    }
    CloseWindow();

    return 0;
}