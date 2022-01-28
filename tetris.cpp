#include<graphics.h>
#include<time.h>
#include<windows.h>
#include<conio.h>
#include<stdio.h>
#include<iostream>
#include<fstream>

/*
1.Create game window
2.Background pic
3.Design tetris
*/

IMAGE imgbg;//background pic
IMAGE imgmn;//manu pic
IMAGE imggo;//gameover pic
IMAGE imgwc;//welcome pic
IMAGE imgps;//pause pic
IMAGE imgal;//arrow left
IMAGE imgar;//arrow right
IMAGE imgab;//about page
int flag;
int sw = 0;
const int selections = 2;//number of menu selections
bool hit = FALSE;//menu hit

/*
block rule :
	[0][1]
	[2][3]
	[4][5]
	[6][7]
*/

int blocks[7][4] = {
	1,3,5,7,//I
	2,4,5,7,//S
	3,4,5,6,//Z
	3,4,5,7,//T
	2,3,5,7,//L
	3,5,6,7,//J
	2,3,4,5,//O
};
struct position {
	int x;//n%2 colum
	int y;//n/2 row
};
//20 rows and 10 columns

const int ROW_COUNT = 20;
const int COL_COUNT = 10;
int map[128][128] = { 0 };//0 = space, n = n_th type of blocks color
const position origin = { 42,86 };//game area left up corner: 42,86
const int width = 38;//block width
const position scoreOrigin = { 463,165 };//scoreboard area left up corner
const position HscoreOrigin = { 463,323 };//highest scoreboard area left up corner
const int numberWidth = 26;//number width
position curBlock[4];//Falling blocks' coordinates
int blockIndex;//type of block(color)
int blockShape[7] = { 0 };//record block shape(vertical or horizontal...)
int score = 0;
int highestScore;
int h;

IMAGE blocksImg[7];//save 7 types of blocks
IMAGE numberImg[10];//save 0-9 numbers
FILE* fp;

void init() {
	
	if ((fp = fopen("./record.txt", "r")) == NULL) //读取1.txt的文件
	{
		printf("读取文件失败 \n ");
		exit(1);
	}
	else {
		while (!feof(fp))
		{
			fscanf(fp,"%d",&highestScore);//每次获取一个字符
		}
		fclose(fp);
	}

	initgraph(640, 900);//according to background pix
	loadimage(&imgbg, "cache/imgbg.jpg");//load image to memory
	loadimage(&imgmn, "cache/menu.jpg");
	loadimage(&imggo, "cache/gameover.jpg");
	loadimage(&imgwc, "cache/welcome.jpg");
	loadimage(&imgps, "cache/pause.jpg");
	loadimage(&imgal, "cache/ArrowL.jpg");
	loadimage(&imgar, "cache/ArrowR.jpg");
	loadimage(&imgab, "cache/about.jpg");
	srand(time(NULL));//random number seed
	//cut blocks image
	IMAGE t1, t2;
	loadimage(&t1, "cache/blocks.jpg");
	SetWorkingImage(&t1);//set blocks image as working image
	for (int i = 0; i < 7; i++)
		getimage(&blocksImg[i], 38 * i, 0, 38, 38);//cut image, left up corner
	SetWorkingImage();
	loadimage(&t2, "cache/number.jpg");
	SetWorkingImage(&t2);
	for (int i = 0; i < 10; i++)
		getimage(&numberImg[i], 26 * i, 0, 26, 50);//cut number image
	SetWorkingImage();//set working image as default
}

void drawBlocks() {
	//fallen static blocks
	for (int i = 0; i < ROW_COUNT; i++) {
		for (int j = 0; j < COL_COUNT; j++) {
			if (map[i][j] == 0)
				continue;
			int x = origin.x + j * width;
			int y = origin.y + i * width;
			int k = map[i][j] - 1;
			putimage(x, y, &blocksImg[k]);
		}
	}
	//falling blocks
	for (int i = 0; i < 4; i++) {
		int x = origin.x + curBlock[i].x * width;
		int y = origin.y + curBlock[i].y * width;
		putimage(x, y, &blocksImg[blockIndex]);
	}
	//score board
	int tem = score;//current score
	for (int i = 4; i >= 0; i--) {
		putimage(scoreOrigin.x + numberWidth * i, scoreOrigin.y, &numberImg[tem % 10]);
		tem /= 10;
	}

	
	if (score > highestScore) {
		fp = fopen("./record.txt", "w");
		highestScore = score;
		fprintf(fp, "%d", highestScore);//save best score to local
		h = highestScore;
	}
	else
		h = highestScore;

	fclose(fp);

	for (int i = 4; i >= 0; i--) {
		putimage(HscoreOrigin.x + numberWidth * i, HscoreOrigin.y, &numberImg[h % 10]);
		h /= 10;
	}
	
}

void moveLeft() {
	for (int i = 0; i < 4; i++)
		curBlock[i].x--;
}

void moveRight() {
	for (int i = 0; i < 4; i++)
		curBlock[i].x++;
}

void moveUp() {
	for (int i = 0; i < 4; i++)
		curBlock[i].y++;
}

void checkBoundary() {
	int flag = 0;
	while (flag < 2) {
		for (int i = 0; i < 4; i++) {
			if (curBlock[i].x > COL_COUNT - 1)
				moveLeft();
			else if (curBlock[i].x < 0)
				moveRight();
			else if (curBlock[i].y > ROW_COUNT - 1 || map[curBlock[i].y][curBlock[i].x] != 0)
				moveUp();
		}
		flag++;
	}
}

void gameover() {
	BeginBatchDraw();
	drawBlocks();
	putimage(53, 410, &imggo);
	EndBatchDraw();
	for (int i = 0; i < ROW_COUNT; i++)//set map to all 0
		for (int j = 0; j < COL_COUNT; j++)
			map[i][j] = 0;
	while (_getch() != 'r')
		;
	score = 0;
}

void generateBlock() {
	blockIndex = rand() % 7;//randomise 1-7
	for (int i = 0; i < 4; i++) {
		int tem = blocks[blockIndex][i];
		curBlock[i].x = tem % 2;
		curBlock[i].y = tem / 2;
	}
	for (int i = 0; i < 4; i++)
		if (map[curBlock[i].y][curBlock[i].x] != 0)//check if lose
			gameover();
}

void saveBlock() {
	for (int i = 0; i < 4; i++) {
		int x = curBlock[i].x;
		int y = curBlock[i].y;
		map[y][x] = blockIndex + 1;
	}
}

void drop() {
	int label = 1;
	for (int i = 0; i < 4; i++) {
		if (curBlock[i].y >= ROW_COUNT - 1 || map[curBlock[i].y + 1][curBlock[i].x] != 0) {//if bottom or blocks underneath, save blocks;
			label = 0;
			saveBlock();
			generateBlock();
			break;
		}
	}
	if (label)//if legal to drop
		for (int i = 0; i < 4; i++)
			curBlock[i].y++;
}

void spinBlock() {
	if (blockIndex == 6)
		return;

	position tem[4];
	for (int i = 0; i < 4; i++) {
		tem[i].x = curBlock[i].x;
		tem[i].y = curBlock[i].y;
	}
	int mark = 1;
	int temx;
	int cenx, ceny;
	cenx = tem[2].x;
	ceny = tem[2].y;
	for (int i = 0; i < 4; i++) {
		if (i == 2)//skip spin center which is not going to move
			continue;
		else {
			temx = tem[i].x;
			tem[i].x = cenx + tem[i].y - ceny;//x spin formula
			tem[i].y = ceny - temx + cenx;//y spin formula
			if (map[tem[i].y][tem[i].x] != 0 || tem[i].y > ROW_COUNT - 1) {
				mark = 0;
				break;
			}
		}
	}
	if (mark)
		for (int i = 0; i < 4; i++) {
			curBlock[i].x = tem[i].x;
			curBlock[i].y = tem[i].y;
		}
}

void keyControl() {
	if (_kbhit()) {
		int label = 1;
		switch (_getch()) {
		case 77://right arrow
			for (int i = 0; i < 4; i++)
				if (curBlock[i].x > COL_COUNT - 2 || map[curBlock[i].y][curBlock[i].x + 1] != 0)
					label = 0;
			if (label)
				moveRight();
			break;
		case 75://left arrow
			for (int i = 0; i < 4; i++)
				if (curBlock[i].x < 1 || map[curBlock[i].y][curBlock[i].x - 1] != 0)
					label = 0;
			if (label)
				moveLeft();
			break;
		case 80://down arrow
			if (curBlock[3].y > ROW_COUNT - 4)//prevent collision between auto drop and down key
				break;
			for (int i = 0; i < 4; i++)
				if (map[curBlock[i].y + 1][curBlock[i].x] != 0) {
					label = 0;
					saveBlock();
					generateBlock();
				}
			if (label)
				for (int i = 0; i < 4; i++) {
					curBlock[i].y++;
					checkBoundary();
				}
			break;
		case 72://up arrow
			spinBlock();
			checkBoundary();
			break;
		case 'r':
			score = 0;
			for (int i = 0; i < ROW_COUNT; i++)
				for (int j = 0; j < COL_COUNT; j++)
					map[i][j] = 0;
			generateBlock();
			break;
		case 'p':
			BeginBatchDraw();
			drawBlocks();
			putimage(53, 410, &imgps);
			EndBatchDraw();
			_getch();
			break;
		}
	}
}

void drawArrow() {

	putimage(445, 480 + sw * 116, &imgal);
	putimage(90, 480 + sw * 116, &imgar);

}

void select() {

	switch (_getch()) {

	case 72://up
		if (sw > 0)
			sw--;
		
		break;

	case 80://down
		if (sw < selections - 1)
			sw++;
		
		break;

	case 13:
		if(sw == 0)
			hit = 1;
		else {
			BeginBatchDraw();
			putimage(0, 0, &imgab);
			EndBatchDraw();
			_getch();
		}

		break;
	}



}

void clearRow(int r) {
	for (int i = r; i >= 0; i--)
		for (int j = 0; j < COL_COUNT; j++) {
			if (i == 0)
				map[i][j] = 0;
			else
				map[i][j] = map[i - 1][j];
		}
}

void checkRow() {
	int sum = 0;
	for (int i = ROW_COUNT - 1; i >= 0; i--) {
		for (int j = COL_COUNT - 1; j >= 0; j--) {
			if (map[i][j] != 0)
				sum++;
		}
		if (sum == 10) {
			clearRow(i);
			score++;
		}
		sum = 0;
	}
}

int main() {
	init();//initiate game
	while (!hit) {
		putimage(0, 0, &imgmn);
		drawArrow();
		select();
	}
	cleardevice();
	putimage(0, 0, &imgbg);//x,y,image file
	putimage(53, 410, &imgwc);
	_getch();
	generateBlock();//generate a new block to start
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgbg);//background image
		keyControl();//get key
		Sleep(10);//delay
		checkBoundary();
		flag++;
		if (flag >= 35) {//automatic drop per 350ms
			drop();
			flag = 0;
		}
		checkRow();
		drawBlocks();//refresh map
		FlushBatchDraw();//prevent flashing
		cleardevice();
	}
	EndBatchDraw();
	system("pause");
	return 0;
}