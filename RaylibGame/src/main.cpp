#include <raylib.h>
#include <cstdint>

// I Don't Know About This 
namespace Score {
	int32_t player_score{};
	int32_t cpu_score{};
}


struct Ball {
	Vector2 m_position;
	int32_t m_speed_x;
	int32_t m_speed_y;
	float m_radius;
	Ball(float x, float y,int32_t ball_speed_x,int32_t ball_speed_y,float radius) 
		:m_position{ x,y }
		,m_speed_x{ball_speed_x}
		,m_speed_y{ball_speed_y}
		,m_radius{radius} {}

	void draw() const {
		DrawCircleV(m_position, m_radius, WHITE);
	}
	void update() {
		m_position.x += m_speed_x;
		m_position.y += m_speed_y;

		// Quite Important To Understand
		if (m_position.y + m_radius >= GetScreenHeight() || m_position.y - m_radius <= 0) {
			m_speed_y *= -1;
		}
		if (m_position.x + m_radius >= GetScreenWidth()) {
			Score::cpu_score++;
			reset_ball();
		}
		if(m_position.x - m_radius <= 0) {
			Score::player_score++;
			reset_ball();
		}
	}
	void reset_ball() {
		m_position.x = GetScreenWidth()/2;
		m_position.y = GetScreenHeight()/2;
		// Otherwise It will Go Same Direction
		m_speed_x *= -1;
	}
};

class Paddle 
{
public:

	Vector2 m_paddle_position;
	int32_t m_speed;
	Vector2 m_size;
public:
	Paddle(float x, float y,int32_t speed,float width,float height) :
		m_paddle_position{ x,y }
		,m_speed{ speed }
		, m_size{ width, height }{}

	void draw() const {
		//DrawRectangleV(m_paddle_position, m_size, WHITE);
		DrawRectangleRounded({ m_paddle_position.x,m_paddle_position.y,m_size.x,m_size.y }, 5, 100, WHITE);
	}
	void update()  {
		if (IsKeyDown(KEY_W)) {
			m_paddle_position.y -= m_speed;
		}
		else if (IsKeyDown(KEY_S)) {
			m_paddle_position.y += m_speed;
		}
		// Hit Floor?
		if (m_paddle_position.y + m_size.y >= GetScreenHeight()) {
			m_paddle_position.y = GetScreenHeight()- m_size.y;
		}
		// Hit Ceiling?
		if (m_paddle_position.y <= 0) {
			m_paddle_position.y = 0;
		}
	}

};
class CpuPaddle :public Paddle {
public:
	// Only Difference is No Input to Move
	CpuPaddle(float x, float y, int32_t speed, float width, float height):
		Paddle{x,y,speed,width,height} {
	}
	void update(const Ball& ball){
		if (ball.m_position.x > GetScreenWidth()/2 || ball.m_speed_x >= 0) {
			return;
		}
		if (ball.m_position.y >= m_paddle_position.y + m_size.y/2) {
			m_paddle_position.y += m_speed;
		}
		else if (ball.m_position.y < m_paddle_position.y + m_size.y / 2) {
			m_paddle_position.y -= m_speed;
		}

		// Hit Floor?
		if (m_paddle_position.y + m_size.y >= GetScreenHeight()) {
			m_paddle_position.y = GetScreenHeight() - m_size.y;
		}
		// Hit Ceiling?
		if (m_paddle_position.y <= 0) {
			m_paddle_position.y = 0;
		}
	}
};

int32_t main() {
	constexpr int32_t window_width = 1920/2;
	constexpr int32_t window_height = 1080/2;

	Ball ball{
		window_width / 2,
		window_height / 2,
		5, // Speed_x
		5, // Speed_y
		10 // Radius
	};

	Paddle right_paddle{
		window_width - window_width / 64,
		window_height / 2,
		5,	// Speed
		window_width/64, // width
		window_height / 4 // height
	};

	CpuPaddle left_paddle{
		0,
		window_height / 2,
		5,	// Speed
		window_width / 64, // width
		window_height / 4 // height
	};



	// The window - OpenGl Context -> Render Api
	InitWindow(window_width, window_height, "Pong Game"); // Works In Linux?
	SetTargetFPS(60); // What Function use this info? EndDrarwing
	// Main loop
	while (!WindowShouldClose()) {

		// Input

		// Update Game
		ball.update();
		right_paddle.update();
		left_paddle.update(ball); // That Count As AI?
		// Why Here? -> The ball and paddles are separate objects,
		// so neither should "own" the collision logic.
		if (CheckCollisionCircleRec(Vector2{ ball.m_position.x,ball.m_position.y },
			ball.m_radius,
			Rectangle{ right_paddle.m_paddle_position.x,
			right_paddle.m_paddle_position.y,
			right_paddle.m_size.x,
			right_paddle.m_size.y })
			||
			CheckCollisionCircleRec(Vector2{ ball.m_position.x,ball.m_position.y },
			ball.m_radius,
			Rectangle{ left_paddle.m_paddle_position.x,
			left_paddle.m_paddle_position.y,
			left_paddle.m_size.x,
			left_paddle.m_size.y })) { ball.m_speed_x *= -1;}

		// Draw
		BeginDrawing();
			ClearBackground(BLANK);
			DrawText(TextFormat("%i", Score::player_score), window_width/1.5, window_height/8, 80, WHITE); // Uses sprintf()
			DrawText(TextFormat("%i", Score::cpu_score),window_width/4,window_height/8,80,WHITE);
			DrawLine(window_width / 2, 0, window_width / 2,window_height,WHITE);
			DrawRing({window_width/2,window_height/2}, 100, 101, 0, 360, 0, WHITE);
			right_paddle.draw();
			left_paddle.draw();
			ball.draw();
		EndDrawing(); // Waits Here
	}

	CloseWindow(); 
	return 0;

}