#include "DxLib.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <cstdlib>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

// シーン
bool title_scene = true;
bool game_scene = false;
bool result_scene = false;
bool home_start = false;
bool game_first_frame = true;

// タイトル移動で使用する変数
float KeyAwaYNG_x = -500.f;
float ShootING_y = -90.f;

// 色設定
unsigned int

// 逃げるボールの色
away_ball_c = GetColor(0, 220, 190), 

// 撃つボールの色
shoot_ball_c = GetColor(250, 100, 230), 

// 背景(通常)の色
back1_c = GetColor(7, 30, 45), 

// 背景の線(通常)の色
back1_line_c = GetColor(0, 80, 70), 

// 背景(ゲームオーバー)の色
back2_c = GetColor(100, 100, 100), 

// 背景の線の色2(ゲームオーバー)
back2_line_c = GetColor(200, 200, 200)
;

int ball_num_def = 5; // 打ち出すボールの最大数
int ball_num = 0; //現在描画されているボールの数
int shoot_count = 0; // 弾を打ち出した回数
int ball_size = 5;


// ブロックの設定

double dir_x, dir_y;



bool click_left; // マウスの左ボタンがクリックされた
bool push_left = false; // マウスの左ボタンが押されている状態
bool pre_push_left = false; // マウスの左ボタンの1フレーム前の状態

bool ClickLeft(int num) {

	click_left = false;

	push_left = (GetKeyState(VK_LBUTTON) & 0x80);

	switch (num) {
		case 0: 
			if (push_left && !pre_push_left) click_left = true;
			break;
		case 1: 
			if (!push_left && pre_push_left) click_left = true;
			break;
	}

	pre_push_left = push_left;
	return click_left;
}

int push_count_up = 0;
int pre_push_count_up;
bool push_key_up;
bool input_up = false;
bool pre_input_up = false;
bool PushUp() {

	push_key_up = false;

	input_up = (GetKeyState(VK_UP) & (0x26 + 0x79));

	if (input_up && !pre_input_up) push_count_up++;
	else if (!input_up && pre_input_up) push_count_up++;

	pre_input_up = input_up;

	if (push_count_up > pre_push_count_up) {
		push_key_up = true;
	}

	pre_push_count_up = push_count_up;

	return push_key_up;
}

int push_count_dw = 0;
int pre_push_count_dw;
bool push_key_dw;
bool input_dw = false;
bool pre_input_dw = false;
bool PushDown() {

	push_key_dw = false;

	input_dw = (GetKeyState(VK_DOWN) & (0x28 + 0x79));

	if (input_dw && !pre_input_dw) push_count_dw++;
	else if (!input_dw && pre_input_dw) push_count_dw++;

	pre_input_dw = input_dw;

	if (push_count_dw > pre_push_count_dw) {
		push_key_dw = true;
	}

	pre_push_count_dw = push_count_dw;

	return push_key_dw;
}


// 背景の線を描画する関数
void BackLine(int num) {
	for (int i = 0; i < 30; i++) {
		// 通常背景
		if (num == 1) {
			DrawLine(0, i * 20, 600, i * 20, back1_line_c);
			DrawLine(i * 20, 0, i * 20, 600, back1_line_c);
		};
		// ゲームオーバー背景
		if (num == 2) {
			DrawLine(0, i * 20, 600, 600 * 20, back2_line_c);
			DrawLine(i * 20, 0, 600 * 20, 600, back2_line_c);
		};
	}
}

// 作成したボタンに応じて、ボタンの上にカーソルが乗っているか判定する
bool TouchButton(int x1, int y1, int x2, int y2, int x, int y) {
	bool x_touch = ((x1 <= x) && (x <= x2));
	bool y_touch = ((y1 <= y) && (y <= y2));
	return (x_touch && y_touch);
}

class Ball {
public :
	double pos_x, pos_y;
	double dir_x, dir_y;

	void WallReflect() {
		if (pos_x >= 600) {
			pos_x = 600;
			dir_x = dir_x * -1;
		}
		if (pos_x <= 0) {
			pos_x = 0;
			dir_x = dir_x * -1;
		}
		if (pos_y >= 600) {
			pos_y = 600;
			dir_y = dir_y * -1;
		}
		if (pos_y <= 0) {
			pos_y = 0;
			dir_y = dir_y * -1;
		}
	}

	void BlockReflect() {

	}
};

class ShootBall : public Ball {
public:

	ShootBall(double x, double y) {
		dir_x = x * 0.06f;
		dir_y = y * 0.06f;
		pos_x = 300;
		pos_y = 600;
	}

	void Move() {
		// ボールを描画

		pos_x = pos_x + dir_x;
		pos_y = pos_y + dir_y;

		WallReflect();

		DrawCircle(pos_x, pos_y, ball_size, shoot_ball_c, TRUE);
	}
};

class KeyAwayBall : public Ball {
private:
	int degree;

public:

	KeyAwayBall() {
		if (rand() % 3 == 1) {
			degree = rand() % 30 + 30;
		}
		else {
			degree = rand() % 30 + 120;
		}
		dir_x = cos(degree * M_PI / 180) * -1;
		dir_y = sin(degree * M_PI / 180);
		dir_x = dir_x * 0.047f;
		dir_y = dir_y * 0.047f;
		pos_x = 300;
		pos_y = 0;
	}

	void Move() {
		// ボールを描画

		WallReflect();
		

		double speed_add = 0.05f;
		if (PushUp()) { // ↑
			if (dir_x < 0) {
				dir_x = dir_x - speed_add;
			}
			else {
				dir_x = dir_x + speed_add;
			}
			if (dir_y < 0) {
				dir_y = dir_y - speed_add;
			}
			else {
				dir_y = dir_y + speed_add;
			}
		}
		if (PushDown()) { // ↓
			if (dir_x < 0) {
				dir_x = dir_x + speed_add;
			}
			else {
				dir_x = dir_x - speed_add;
			}
			if (dir_y < 0) {
				dir_y = dir_y + speed_add;
			}
			else {
				dir_y = dir_y - speed_add;
			}
		}

		pos_x = pos_x + dir_x;
		pos_y = pos_y + dir_y;

		DrawCircle(pos_x, pos_y, ball_size, away_ball_c, TRUE);
	}
};

double away_physics_x[60];
double away_physics_y[60];
double shoot_physics_x[60];
double shoot_physics_y[60];

void GiveBallPhysics(double physics_x[], double physics_y[], double x, double y) {
	for (int i = 0; i < 60; i++) {
		physics_x[i] = x + ball_size * cos(i * 6);
		physics_y[i] = y + ball_size * sin(i * 6);
	}
}

void Reset() {
	KeyAwaYNG_x = -500.f;
	ShootING_y = -90.f;

	ball_num_def = 5;
	ball_num = 0;
	shoot_count = 0;

	bool push_left = false;
	bool pre_push_left = false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (DxLib_Init() == -1) { return -1; }

	/*------------ set up --------------*/

	// 画面サイズ
	ChangeWindowMode(true);
	SetGraphMode(600, 600, 32);

	// フレームレート
	LONGLONG now;
	LONGLONG was = GetNowCount();
	float framerate;

	// フォント設定
	int title_font = CreateFontToHandle(NULL, 100, 6);
	int count_font = CreateFontToHandle(NULL, 60, 6);
	int next_font = CreateFontToHandle(NULL, 30, 6);

	// マウスポインタを表示
	SetMouseDispFlag(TRUE);
	int mouse_x, mouse_y;

	// 角度決定
	double degree; // 角度
	double x_dir = 0; // 
	double y_dir = 0; // 
	double first_x_dir = 0;
	double first_y_dir = 0;

	// ShootBallを格納しておく配列を作成
	vector<ShootBall> shoot_ball;

	KeyAwayBall away_ball;
	away_ball = KeyAwayBall();


	/*------------- main ---------------*/

	while (ProcessMessage() == 0)
	{
		ClearDrawScreen();

		// ゲームを進めるフレームレートを作成
		now = GetNowCount();
		framerate = (now - was) / 1.0f; // 1秒に1000回更新する
		was = now;

		// マウス関係の処理
		GetMousePoint(&mouse_x, &mouse_y);

		// タイトル画面
		if (title_scene) {

			// 背景を描画
			DrawBox(0, 0, 600, 600, back1_c, TRUE);
			BackLine(1);


			/*------ ゲームタイトルの描画 ------*/

			// ゲームタイトルを描画
			DrawStringToHandle((int)KeyAwaYNG_x, 140, "KeyAwaYNG", away_ball_c, title_font);
			DrawStringToHandle(97, (int)ShootING_y, "ShootING", shoot_ball_c, title_font);

			// ゲームタイトルの移動が完了するまで
			if (ShootING_y <= 62) {
				if (KeyAwaYNG_x <= 71.0f) KeyAwaYNG_x += 0.2f * framerate;
				if (KeyAwaYNG_x >= 39.0f) ShootING_y += 0.8f * framerate;
			}
			else home_start = true;


			// ゲームタイトルの移動が完了したら
			if (home_start) {


				/*---- ボールの個数を決定するボタン ----*/

				// ＋ボタン
				if (TouchButton(160, 320, 240, 400, mouse_x, mouse_y)) {
					if ((ball_num_def < 999)) {
						// ボタン
						DrawBox(160, 320, 240, 400, GetColor(200, 110, 124), TRUE);
						// +
						DrawStringToHandle(175, 311, "+", GetColor(255, 0, 0), title_font);
						// 枠
						for (int i = 0; i < 4; i++) {
							DrawBox(160+i, 320+i, 240-i, 400-i, GetColor(255, 0, 0), FALSE);
						}
						if (ClickLeft(0)) ball_num_def++;
					}
					else {
						// +
						DrawStringToHandle(175, 311, "+", GetColor(200, 110, 124), title_font);
						// 枠
						for (int i = 0; i < 4; i++) {
							DrawBox(160+i, 320+i, 240-i, 400-i, GetColor(200, 110, 124), FALSE);
						}
					}
				}
				else{
					DrawStringToHandle(175, 311, "+", GetColor(200, 110, 124), title_font);
					for (int i = 0; i < 4; i++) {
						DrawBox(160+i, 320+i, 240-i, 400-i, GetColor(200, 110, 124), FALSE);
					}
				}

				// －ボタン
				if (TouchButton(360, 320, 440, 400, mouse_x, mouse_y)) {
					if ((ball_num_def > 1)) {
						// ボタン
						DrawBox(360, 320, 440, 400, GetColor(152, 125, 200), TRUE);
						// -
						DrawStringToHandle(376, 311, "-", GetColor(0, 0, 255), title_font);
						// 枠
						for (int i = 0; i < 4; i++) {
							DrawBox(360+i, 320+i, 440-i, 400-i, GetColor(0, 0, 255), FALSE);
						}
						if (ClickLeft(0)) ball_num_def--;
					}
					else {
						// -
						DrawStringToHandle(376, 311, "-", GetColor(152, 125, 200), title_font);
						// 枠
						for (int i = 0; i < 4; i++) {
							DrawBox(360+i, 320+i, 440-i, 400-i, GetColor(152, 125, 200), FALSE);
						}
					}
				}
				else {
					DrawStringToHandle(376, 311, "-", GetColor(152, 125, 200), title_font);
					for (int i = 0; i < 4; i++) {
						DrawBox(360+i, 320+i, 440-i, 400-i, GetColor(152, 125, 200), FALSE);
					}
				}

				// ボールの数
				if (ball_num_def < 10) DrawFormatStringToHandle(285, 332, shoot_ball_c, count_font, "%d", ball_num_def);
				else if(ball_num_def < 100) DrawFormatStringToHandle(270, 332, shoot_ball_c, count_font, "%d", ball_num_def);
				else DrawFormatStringToHandle(254, 332, shoot_ball_c, count_font, "%d", ball_num_def);
				for (int i = 0; i < 3; i++) {
					DrawBox(240+i, 320+i, 360-i, 400-i, shoot_ball_c, FALSE);
				}
				DrawString(247, 286, "Shoot Ball's", shoot_ball_c);
				DrawString(293, 303, "×", shoot_ball_c);


				/*------- スタートボタン -------*/

				// スタートボタンを描画
				DrawBox(100, 440, 500, 540, away_ball_c, TRUE);

				// スタートボタンがクリックされたら
				if (TouchButton(100, 440, 500, 540, mouse_x, mouse_y) && ClickLeft(1)) {
					game_scene = true; // ゲーム開始
					title_scene = false; // タイトル画面終了
					home_start = false;
				} 
			}
		}

		if (game_scene) {

			// 背景を描画
			DrawBox(0, 0, 600, 600, back1_c, TRUE);
			BackLine(1);

			// KeyAwayballを描画
			away_ball.Move();

			if (push_left) {

				// 照準と打ち出すボールを描画
				DrawLine(300, 600, mouse_x, mouse_y, GetColor(255, 255, 255));
				DrawCircle(300, 600, ball_size, shoot_ball_c, TRUE);
			}
			if (ClickLeft(1)) {

				// 打ち出す位置とカーソルの位置の角度を計算
				degree = ((atan2(300 - mouse_x, 600 - mouse_y) * 180 / M_PI) - 90); 
				x_dir = cos(degree * M_PI / 180) * -1;
				y_dir = sin(degree * M_PI / 180);

				if (ball_num < ball_num_def) {
					// ShootBallの数が設定した数に満たない場合
					shoot_ball.push_back(ShootBall(x_dir, y_dir));
					ball_num++;
				}
				else {
					// ShootBallの数が設定した数の場合
					shoot_ball[shoot_count % ball_num_def] = ShootBall(x_dir, y_dir);
				}

				shoot_count++;
			}

			// shootballを描写
			for (int i = 0; i < ball_num; i++) {
				shoot_ball[i].Move();
			}

			// 当たり判定
			GiveBallPhysics(away_physics_x, away_physics_y, away_ball.pos_x, away_ball.pos_y);
			for (int ball = 0; ball < ball_num; ball++) {
				GiveBallPhysics(shoot_physics_x, shoot_physics_y, shoot_ball[ball].pos_x, shoot_ball[ball].pos_y);

				// awayball が shootball より上にある場合
				bool away_up = away_ball.pos_y < shoot_ball[ball].pos_y;
				bool away_under = away_ball.pos_y > shoot_ball[ball].pos_y;
				bool away_right = away_ball.pos_x > shoot_ball[ball].pos_x;
				bool away_left = away_ball.pos_x < shoot_ball[ball].pos_x;

				bool col_up_l_x, col_up_l_y;
				bool col_up_r_x, col_up_r_y;
				bool col_under_l_x, col_under_l_y;
				bool col_under_r_x, col_under_r_y;

				for (int away_ang = 0; away_ang < 60; away_ang++) {
					for (int shoot_ang = 0; shoot_ang < 60; shoot_ang++) {

						col_up_l_x = shoot_physics_x[shoot_ang] <= away_physics_x[away_ang];
						col_up_l_y = shoot_physics_y[shoot_ang] <= away_physics_y[away_ang];
						col_up_r_x = shoot_physics_x[shoot_ang] >= away_physics_x[away_ang];
						col_up_r_y = shoot_physics_y[shoot_ang] <= away_physics_y[away_ang];
						col_under_l_x = shoot_physics_x[shoot_ang] <= away_physics_x[away_ang];
						col_under_l_y = shoot_physics_y[shoot_ang] >= away_physics_y[away_ang];
						col_under_r_x = shoot_physics_x[shoot_ang] >= away_physics_x[away_ang];
						col_under_r_y = shoot_physics_y[shoot_ang] >= away_physics_y[away_ang];

						if (shoot_ang >= 0 && shoot_ang <= 14) {
							if (away_up && away_right) {
								if (col_up_r_x && col_up_r_y) {
									game_scene = false;
									result_scene = true;
								}
							}
						}
						else if (shoot_ang >= 15 && shoot_ang <= 29) {
							if (away_under && away_right) {
								if (col_under_r_x && col_under_r_y) {
									game_scene = false;
									result_scene = true;
								}
							}
						}
						else if (shoot_ang >= 30 && shoot_ang <= 44) {
							if (away_under && away_left) {
								if (col_under_l_x && col_under_l_y) {
									game_scene = false;
									result_scene = true;
								}
							}
						}
						else if (shoot_ang >= 45 && shoot_ang <= 59) {
							if (away_up && away_left) {
								if (col_up_l_x && col_up_l_y) {
									game_scene = false;
									result_scene = true;
								}
							}
						}
					}
				}
			}
		}

		if (result_scene) {
			DrawFormatStringFToHandle(100, 250, GetColor(255, 0, 0), title_font, "Game Over");
			DrawFormatStringToHandle(50, 400, GetColor(255, 255, 255), next_font, "Go title :  mouse click");

			if (ClickLeft(1)) {
				result_scene = false;
				title_scene = true;

				away_ball = KeyAwayBall();
				shoot_ball.clear();
				Reset();
			}
		}
	}

	DxLib_End();
	return 0;
}

// クラスの宣言から