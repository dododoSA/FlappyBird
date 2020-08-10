#include "DxLib.h"

//=============�萔==============
#define BIRD_SIZE 64 //���̉摜�T�C�Y
#define G 0.05 //�������x�̕ω���
#define JUMP_SPEED -3.0 //�W�����v���鐨��

#define BACKGROUND_WIDTH 1234 //�w�i�摜�̉���
#define MOVE_SPEED 0.5 //���X�N���[���̑���

#define WALL_WIDTH 86 //��Q���̉摜�̕�
#define WALL_HEIGHT 271 //��Q���̉摜�̍���
#define WALL_GAP 170 //��Q���̌���
#define WALL_NUM 10 //�Ǘ������Q���̐�
#define WALL_INTERVAL 480 //�o���Ԋu(�t���[����)

int scene; //�V�[���Ǘ��p�ϐ�
void init(); //�v���g�^�C�v�錾

//=============�L�[�{�[�h�֘A============
int key[256]; //�L�[�������Ă��牽�t���[���o�߂����̂����J�E���g���邽�߂̔z��
char buf[256]; //�L�[�̉�����Ԃ�ۑ����邽�߂̕ϐ�

void InputKeyboard() {
	GetHitKeyStateAll(buf);
	//�S�ẴC���f�b�N�X�Ɋւ��Ē��ׂ�
	for (int i = 0; i < 256; i++) {
		//�����L�[��������Ă�����J�E���g��1���₷
		if (buf[i] == 1) {
			key[i]++;
		}
		//����������Ă��Ȃ�������J�E���g��0�ɂ���
		else {
			key[i] = 0;
		}
	}
}

//=============���ɂ��Ă̍\����===========
struct Bird
{
	double x, y;
	double speed_y;
	int graph_handle;
};

Bird bird; //�O���[�o���ϐ��Ƃ��Đ錾

//=============���̕`��p�֐�=============
void DrawBird() {
	DrawGraph(bird.x - BIRD_SIZE / 2, bird.y - BIRD_SIZE / 2, bird.graph_handle, TRUE); //�摜��`��(�����)
}

//=============���̍X�V�p�֐�=============
void UpdateBird() {
	bird.y += bird.speed_y; //���x�̕���������������

	bird.speed_y += G; //�������x�����񂾂񑬂��Ȃ�

	//== 1�Ȃ̂ŃX�y�[�X�L�[�������ꂽ�u�Ԃ̂Ƃ�����true
	if (key[KEY_INPUT_SPACE] == 1) {
		bird.speed_y = JUMP_SPEED; //�W�����v���̑��x�ɂ���
	}

	if (bird.y > 480) { //���ɗ�������
		scene = 0; //�^�C�g����ʂɂ���
		init(); //�Y�ꂸ������Ԃɂ���
	}
}

//=============�w�i�֘A=============
struct Background {
	double x; //x���W
	double y; //y���W
	int graph_handle; //�O���t�B�b�N�n���h��
};

Background bg[2]; //��p�ӂ���

void UpdateBackground() {
	bg[0].x -= MOVE_SPEED;
	bg[1].x -= MOVE_SPEED;

	//���S�ɂ͂ݏo������
	if (bg[0].x <= -BACKGROUND_WIDTH) {
		//��������̔w�i�̂������Ɉړ�
		bg[0].x = bg[1].x + BACKGROUND_WIDTH;
	}

	if (bg[1].x <= -BACKGROUND_WIDTH) {
		bg[1].x = bg[0].x + BACKGROUND_WIDTH;
	}
}

void DrawBackground() {
	DrawGraph(bg[0].x, bg[0].y, bg[0].graph_handle, TRUE); //�w�i1��\��
	DrawGraph(bg[1].x, bg[1].y, bg[1].graph_handle, TRUE); //�w�i2��\��
}

//=============��Q���֘A================
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
	wall_count++; //1�t���[�����Ƃ�1����

	if (wall_count > WALL_INTERVAL) { //���t���[���o�߂�����
		wall_count = 0; //�J�E���g�����Z�b�g

		for (int i = 0; i < WALL_NUM; i++) {
			if (walls[i].draw_flag == false) { //�����\������Ă��Ȃ���Q������������
				walls[i].pass_flag = false; //�ʉ߂��Ă��Ȃ�
				walls[i].draw_flag = true; //�\������
				walls[i].x = 640; //�E�[�ɐݒu
				walls[i].y = GetRand(280 - WALL_GAP) + 100 - WALL_HEIGHT; //���ԕ�����y���W��100�`380�̊ԂɎ��܂�悤�ɐݒ�
				break; //�\�����������̂�1�Ȃ̂Ń��[�v��E�o
			}
		}
	}

	for (int i = 0; i < WALL_NUM; i++) {
		if (walls[i].draw_flag == true) {
			walls[i].x -= MOVE_SPEED;

			//�������E�ɂ���A�Ȃ����܂��ʉ߂��Ă��Ȃ���Q����������
			if (walls[i].x + WALL_WIDTH < bird.x - BIRD_SIZE / 2 && walls[i].pass_flag == false) {
				walls[i].pass_flag = true; //�ʉ߂������Ƃ��L�^
				score++; //�X�R�A�����Z
			}

			//��ʂ����������
			if (walls[i].x < -WALL_WIDTH) {
				walls[i].draw_flag = false; //�\�������Ȃ�
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

//=============�����蔻��==============

void CollisionDetection() {
	for (int i = 0; i < WALL_NUM; i++) {
		if (walls[i].draw_flag == true) {
			if (
				walls[i].x <= bird.x + BIRD_SIZE / 2 && walls[i].x + WALL_WIDTH >= bird.x - BIRD_SIZE / 2 && /*�������Ɋւ��ď�Q���ɐG��Ă��邩�ǂ���*/
				(walls[i].y + WALL_HEIGHT >= bird.y - BIRD_SIZE / 2 || walls[i].y + WALL_HEIGHT + WALL_GAP <= bird.y + BIRD_SIZE / 2) /*��̏�Q���ɐG��Ă���܂��͉��̏�Q���ɐG��Ă���*/
				) {
				scene = 0; //�^�C�g����ʂɂ���
				init(); //�Y�ꂸ������Ԃɂ���
			}
		}
	}

}

//=============�Q�[���̏������p�֐�==============
void init() {
	//���̍��W
	bird.x = 250;
	bird.y = 240;

	//���̗������x
	bird.speed_y = 0;

	//�w�i�̍��W
	bg[0].x = 0;
	bg[0].y = 0;
	bg[1].x = BACKGROUND_WIDTH;
	bg[1].y = 0;

	//��
	for (int i = 0; i < WALL_NUM; i++) {
		walls[i].x = 640;
		walls[i].y = 0;
		walls[i].pass_flag = false;
	}

	wall_count = 0;

	score = 0;

	scene = 0; //�X�^�[�g��ʂɂ���
}

//=============�X�V=============
void UpdateAll() {
	if (scene == 0) {
		//�X�^�[�g���
		if (key[KEY_INPUT_SPACE] > 0) { //�X�y�[�X�L�[�������ꂽ��
			scene = 1; //�V�[�����v���C��ʂɕς���
		}
	}
	else {
		//�v���C���
		UpdateBackground();
		UpdateWall();
		UpdateBird();
		CollisionDetection();
	}
}

//=============�`��=============
void DrawAll() {
	DrawBackground();
	DrawWall();
	DrawBird();
	DrawFormatString(0, 0, GetColor(0, 0, 0), "%d", score); //�X�R�A�\��

	//�X�^�[�g��ʂ�������uPUSH SPACE�v��\��
	if (scene == 0) {
		DrawString(100, 100, "PUSH SPACE", GetColor(0, 0, 0));
	}
}

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); //�E�B���h�E���[�h�ŋN��

	SetGraphMode(640, 480, 32); //��ʃT�C�Y���w��


	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	SetDrawScreen(DX_SCREEN_BACK); //����ʂɕ`��

	SetFontSize(48); //�X�R�A�̃t�H���g�T�C�Y


	//=============�摜��ǂݍ���=================
	bird.graph_handle = LoadGraph("img/bird.png");
	bg[0].graph_handle = LoadGraph("img/��.png");
	bg[1].graph_handle = bg[0].graph_handle;
	wall_ghandle = LoadGraph("img/��1.png");

	init(); //������


	// �G���[�����E�E�B���h�E��������܂Ń��[�v
	while (ProcessMessage() != -1) {
		ClearDrawScreen(); //��ʂ��N���A����
		InputKeyboard();

		UpdateAll();

		DrawAll();


		ScreenFlip(); //����ʂ�\�ɔ��f
	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I��
}