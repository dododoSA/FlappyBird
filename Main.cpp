#include "DxLib.h"

//=============定数==============
#define BIRD_SIZE 64 //鳥の画像サイズ
#define G 0.05 //落下速度の変化量
#define JUMP_SPEED -3.0 //ジャンプする勢い

#define BACKGROUND_WIDTH 1234 //背景画像の横幅
#define MOVE_SPEED 0.5 //横スクロールの速さ

#define WALL_WIDTH 86 //障害物の画像の幅
#define WALL_HEIGHT 271 //障害物の画像の高さ
#define WALL_GAP 170 //障害物の隙間
#define WALL_NUM 10 //管理する障害物の数
#define WALL_INTERVAL 480 //出現間隔(フレーム数)

int scene; //シーン管理用変数
void init(); //プロトタイプ宣言

//=============キーボード関連============
int key[256]; //キーを押してから何フレーム経過したのかをカウントするための配列
char buf[256]; //キーの押下状態を保存するための変数

void InputKeyboard() {
	GetHitKeyStateAll(buf);
	//全てのインデックスに関して調べる
	for (int i = 0; i < 256; i++) {
		//もしキーが押されていたらカウントを1増やす
		if (buf[i] == 1) {
			key[i]++;
		}
		//もし押されていなかったらカウントを0にする
		else {
			key[i] = 0;
		}
	}
}

//=============鳥についての構造体===========
struct Bird
{
	double x, y;
	double speed_y;
	int graph_handle;
};

Bird bird; //グローバル変数として宣言

//=============鳥の描画用関数=============
void DrawBird() {
	DrawGraph(bird.x - BIRD_SIZE / 2, bird.y - BIRD_SIZE / 2, bird.graph_handle, TRUE); //画像を描画(中央基準)
}

//=============鳥の更新用関数=============
void UpdateBird() {
	bird.y += bird.speed_y; //速度の分だけ落下させる

	bird.speed_y += G; //落下速度がだんだん速くなる

	//== 1なのでスペースキーが押された瞬間のときだけtrue
	if (key[KEY_INPUT_SPACE] == 1) {
		bird.speed_y = JUMP_SPEED; //ジャンプ時の速度にする
	}

	if (bird.y > 480) { //下に落ちたら
		scene = 0; //タイトル画面にする
		init(); //忘れず初期状態にする
	}
}

//=============背景関連=============
struct Background {
	double x; //x座標
	double y; //y座標
	int graph_handle; //グラフィックハンドル
};

Background bg[2]; //二つ用意する

void UpdateBackground() {
	bg[0].x -= MOVE_SPEED;
	bg[1].x -= MOVE_SPEED;

	//完全にはみ出したら
	if (bg[0].x <= -BACKGROUND_WIDTH) {
		//もう一方の背景のすぐ後ろに移動
		bg[0].x = bg[1].x + BACKGROUND_WIDTH;
	}

	if (bg[1].x <= -BACKGROUND_WIDTH) {
		bg[1].x = bg[0].x + BACKGROUND_WIDTH;
	}
}

void DrawBackground() {
	DrawGraph(bg[0].x, bg[0].y, bg[0].graph_handle, TRUE); //背景1を表示
	DrawGraph(bg[1].x, bg[1].y, bg[1].graph_handle, TRUE); //背景2を表示
}

//=============障害物関連================
struct Wall {
	double x;
	double y;
	bool draw_flag;
	bool pass_flag;
};
Wall walls[WALL_NUM];

int wall_ghandle;
int wall_count;
int score;

void UpdateWall() {
	wall_count++; //1フレームごとに1増加

	if (wall_count > WALL_INTERVAL) { //一定フレーム経過したら
		wall_count = 0; //カウントをリセット

		for (int i = 0; i < WALL_NUM; i++) {
			if (walls[i].draw_flag == false) { //もし表示されていない障害物があったら
				walls[i].pass_flag = false; //通過していない
				walls[i].draw_flag = true; //表示する
				walls[i].x = 640; //右端に設置
				walls[i].y = GetRand(280 - WALL_GAP) + 100 - WALL_HEIGHT; //隙間部分のy座標が100～380の間に収まるように設定
				break; //表示させたいのは1つなのでループを脱出
			}
		}
	}

	for (int i = 0; i < WALL_NUM; i++) {
		if (walls[i].draw_flag == true) {
			walls[i].x -= MOVE_SPEED;

			//鳥よりも右にあり、なおかつまだ通過していない障害物だったら
			if (walls[i].x + WALL_WIDTH < bird.x - BIRD_SIZE / 2 && walls[i].pass_flag == false) {
				walls[i].pass_flag = true; //通過したことを記録
				score++; //スコアを加算
			}

			//画面から消えたら
			if (walls[i].x < -WALL_WIDTH) {
				walls[i].draw_flag = false; //表示させない
			}
		}
	}
}

void DrawWall() {
	for (int i = 0; i < WALL_NUM; i++) {
		if (walls[i].draw_flag == true) {
			DrawGraph(walls[i].x, walls[i].y, wall_ghandle, TRUE);
			DrawGraph(walls[i].x, walls[i].y + WALL_HEIGHT + WALL_GAP, wall_ghandle, TRUE);
		}
	}
}

//=============当たり判定==============

void CollisionDetection() {
	for (int i = 0; i < WALL_NUM; i++) {
		if (walls[i].draw_flag == true) {
			if (
				walls[i].x <= bird.x + BIRD_SIZE / 2 && walls[i].x + WALL_WIDTH >= bird.x - BIRD_SIZE / 2 && /*横方向に関して障害物に触れているかどうか*/
				(walls[i].y + WALL_HEIGHT >= bird.y - BIRD_SIZE / 2 || walls[i].y + WALL_HEIGHT + WALL_GAP <= bird.y + BIRD_SIZE / 2) /*上の障害物に触れているまたは下の障害物に触れている*/
				) {
				scene = 0; //タイトル画面にする
				init(); //忘れず初期状態にする
			}
		}
	}

}

//=============ゲームの初期化用関数==============
void init() {
	//鳥の座標
	bird.x = 250;
	bird.y = 240;

	//鳥の落下速度
	bird.speed_y = 0;

	//背景の座標
	bg[0].x = 0;
	bg[0].y = 0;
	bg[1].x = BACKGROUND_WIDTH;
	bg[1].y = 0;

	//壁
	for (int i = 0; i < WALL_NUM; i++) {
		walls[i].x = 640;
		walls[i].y = 0;
		walls[i].pass_flag = false;
	}

	wall_count = 0;

	score = 0;

	scene = 0; //スタート画面にする
}

//=============更新=============
void UpdateAll() {
	if (scene == 0) {
		//スタート画面
		if (key[KEY_INPUT_SPACE] > 0) { //スペースキーが押されたら
			scene = 1; //シーンをプレイ画面に変える
		}
	}
	else {
		//プレイ画面
		UpdateBackground();
		UpdateWall();
		UpdateBird();
		CollisionDetection();
	}
}

//=============描画=============
void DrawAll() {
	DrawBackground();
	DrawWall();
	DrawBird();
	DrawFormatString(0, 0, GetColor(0, 0, 0), "%d", score); //スコア表示

	//スタート画面だったら「PUSH SPACE」を表示
	if (scene == 0) {
		DrawString(100, 100, "PUSH SPACE", GetColor(0, 0, 0));
	}
}

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); //ウィンドウモードで起動

	SetGraphMode(640, 480, 32); //画面サイズを指定


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK); //裏画面に描画

	SetFontSize(48); //スコアのフォントサイズ


	//=============画像を読み込み=================
	bird.graph_handle = LoadGraph("img/bird.png");
	bg[0].graph_handle = LoadGraph("img/青空.png");
	bg[1].graph_handle = bg[0].graph_handle;
	wall_ghandle = LoadGraph("img/壁1.png");

	init(); //初期化


	// エラー発生・ウィンドウが閉じられるまでループ
	while (ProcessMessage() != -1) {
		ClearDrawScreen(); //画面をクリアする
		InputKeyboard();

		UpdateAll();

		DrawAll();


		ScreenFlip(); //裏画面を表に反映
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了
}