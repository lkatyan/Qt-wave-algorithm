#include "paint.h"
#include "ui_paint.h"

const int W = 12;         // ширина рабочего поля
const int H = 8;         // высота рабочего поля
char result[H][W];
int grid[H][W];
int px[W * H], py[W * H]; // координаты ячеек, входящих  путь
int karta[H][W] = { {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
                   {-1, -3, -2, -2, -2, -2, -2, -2, -2, -2, -3, -1 },
                   {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1 },
                   {-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1 },
                   {-1, -2, -2, -2, -2, -3, -2, -1, -2, -2, -2, -1 },
                   {-1, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -1 },
                   {-1, -2, -2, -2, -2, -2, -2, -1, -3, -2, -2, -1 },
                   {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, };

bool alghorithm(int ax, int ay, int bx, int by)   // поиск пути из ячейки (ax, ay) в ячейку (bx, by)
{
    int grid[H][W];
    int px[W * H], py[W * H];
    int len;                       // длина пути
    const int WALL = -1;         // непроходимая ячейка
    const int BLANK = -2;         // свободная непомеченная ячейка
    const int POINT = -3;
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            grid[i][j] = karta[i][j];
    int dx[4] = { 1, 0, -1, 0 };   // смещения, соответствующие соседям ячейки
    int dy[4] = { 0, 1, 0, -1 };   // справа, снизу, слева и сверху
    int d, x, y, k;
    bool stop = false;

    if (grid[ay][ax] == WALL || grid[by][bx] == WALL) return false;  // ячейка (ax, ay) или (bx, by) - стена

    // распространение волны
    d = 0;
    grid[ay][ax] = 0;            // стартовая ячейка помечена 0
    while (!stop && grid[by][bx] == POINT)
    {
        stop = true;               // предполагаем, что все свободные клетки уже помечены
        for (y = 0; y < H; ++y)
            for (x = 0; x < W; ++x)
                if (grid[y][x] == d)                         // ячейка (x, y) помечена числом d
                {
                    for (k = 0; k < 4; ++k)                    // проходим по всем непомеченным соседям
                    {
                        int iy = y + dy[k], ix = x + dx[k];
                        if (iy >= 0 && iy < H && ix >= 0 && ix < W && (grid[iy][ix] == BLANK||grid[iy][ix] ==POINT))
                        {
                            stop = false;              // найдены непомеченные клетки
                            grid[iy][ix] = d + 1;      // распространяем волну
                        }
                    }
                }
        d++;
    }

    if (grid[by][bx] == POINT) return false;  // путь не найден

    // восстановление пути
    len = grid[by][bx];            // длина кратчайшего пути из (ax, ay) в (bx, by)
    x = bx;
    y = by;
    d = len;
    while (d >= 0)
    {
        px[d] = x;
        py[d] = y;                   // записываем ячейку (x, y) в путь
        d--;
        for (k = 0; k < 4; ++k)
        {
            int iy = y + dy[k], ix = x + dx[k];
            if (iy >= 0 && iy < H && ix >= 0 && ix < W &&
                grid[iy][ix] == d)
            {
                x = x + dx[k];
                y = y + dy[k];           // переходим в ячейку, которая на 1 ближе к старту
                break;
            }
        }
    }
    //px[0] = ax;
    //py[0] = ay;                    // теперь px[0..len] и py[0..len] - координаты ячеек пути
    for (int i = 0; i <= len; i++) {
        result[py[i]][px[i]] = 47;
    }
    result[ay][ax] = 35;
    result[by][bx] = 35;
    return true;
}

Paint::Paint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Paint)
{
    ui->setupUi(this);

    scene = new paintScene();       // Инициализируем графическую сцену
    ui->graphicsView->setScene(scene);  // Устанавливаем графическую сцену

    timer = new QTimer();       // Инициализируем таймер
    connect(timer, &QTimer::timeout, this, &Paint::slotTimer);
    timer->start(100);          // Запускаем таймер


    // Создание Таблицы

     ui->tableWidget->setRowCount(H); // указываем количество строк
     ui->tableWidget->setColumnCount(W); // указываем количество столбцов

     // выделяем память под все ячейки таблицы
     for(int stroka = 0; stroka <   ui->tableWidget->rowCount(); stroka++)
     for(int stolbeu = 0; stolbeu <   ui->tableWidget->columnCount(); stolbeu++)

         ui->tableWidget->setItem(stroka, stolbeu, new QTableWidgetItem());


     // выделяем память под все ячейки таблицы
     for(int stroka = 0; stroka < ui->tableWidget->rowCount(); stroka++)
       for(int stolbeu = 0; stolbeu < ui->tableWidget->columnCount(); stolbeu++)
       {
            QTableWidgetItem *item = new QTableWidgetItem(); // выделяем память под ячейку
            item->setText(QString("%1").arg(karta[stroka][stolbeu])); // вставляем текст

            ui->tableWidget->setItem(stroka, stolbeu, item); // вставляем ячейку
       }


     int x[W * H], y[W * H]; // массивы с точками для обхода
     int count=0;
     for (int j = 0; j < W; j++) {      // заполняем массив значениями с карты
         for (int i = 0; i < H; i++) {
             if (karta[i][j] == -1) result[i][j] = 45;
             else if (karta[i][j] == -2) result[i][j] = 42;
             else if (karta[i][j] == -3) {
                 x[count] = j;
                 y[count] = i;
                 count++;
                 result[i][j] = 35;
             }
         }
     }

     for (int i = 0; i < count-1; i++) {
         alghorithm(x[i], y[i], x[i + 1], y[i + 1]);
     }

     for (int i = 0; i < H; i++) {
         for (int j = 0; j < W; j++)
             if (result[i][j]==35)
                 scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(QColor(Qt::cyan).lighter(85)));
         else if (result[i][j]==47)
             scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(Qt::darkCyan));
         else if (result[i][j]==45)
             scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(Qt::black));
         else if (result[i][j]==42)
            scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(Qt::lightGray));
     }
}

Paint::~Paint()
{
    delete ui;
}

void Paint::slotTimer()
{
    /* Переопределяем размеры графической сцены в зависимости
     * от размеров окна
     * */
    timer->stop();
    scene->setSceneRect(0,0, ui->graphicsView->width() - 40, ui->graphicsView->height() - 40);
}

void Paint::resizeEvent(QResizeEvent *event)
{
    timer->start(100);
    QWidget::resizeEvent(event);
}

void Paint::on_pushButton_clicked()
{
    int i;
    for(int stroka = 0; stroka < ui->tableWidget->rowCount(); stroka++)
      for(int stolbeu = 0; stolbeu < ui->tableWidget->columnCount(); stolbeu++)
      {
          i=ui->tableWidget->item(stroka,stolbeu)->data(Qt::DisplayRole).toInt();
          karta[stroka][stolbeu]=i;
      }
    scene->update();
    int x[W * H], y[W * H]; // массивы с точками для обхода
    int count=0;
    for (int j = 0; j < W; j++) {      // заполняем массив значениями с карты
        for (int i = 0; i < H; i++) {
            if (karta[i][j] == -1) result[i][j] = 45;
            else if (karta[i][j] == -2) result[i][j] = 42;
            else if (karta[i][j] == -3) {
                x[count] = j;
                y[count] = i;
                count++;
                result[i][j] = 35;
            }
        }
    }

    for (int i = 0; i < count-1; i++) {
        alghorithm(x[i], y[i], x[i + 1], y[i + 1]);
    }

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++)
            if (result[i][j]==35)
                scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(QColor(Qt::cyan).lighter(85)));
        else if (result[i][j]==47)
            scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(Qt::darkCyan));
        else if (result[i][j]==45)
            scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(Qt::black));
        else if (result[i][j]==42)
           scene->addRect(QRect(j*30, i*30, 25, 25 ), QPen(Qt::NoPen), QBrush(Qt::lightGray));
    }
}
