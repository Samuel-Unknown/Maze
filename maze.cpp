#include "maze.h"

Maze::Maze(QWidget *parent) :
    QWidget(parent)
{
    cells_h = 20;
    cells_v = 20;

    width = 20;
    height = 20;

    shiftForPoints = 2;

    x1 = shiftForPoints;
    y1 = shiftForPoints;
    x2 = cells_h * width - (width - shiftForPoints);
    y2 = cells_v * height - (height - shiftForPoints);

    path_found = false;

    // Выделяем память
     alg_Li = new int*[cells_h];
     for (int i = 0; i < cells_h; i++)
         alg_Li[i] = new int[cells_v];

     cells = new Square** [cells_h];
     for (int i = 0; i < cells_h; i++)
         cells[i] = new Square* [cells_v];

     for (int i = 0; i < cells_v; i++)
        for (int j = 0; j < cells_h; j++)
            cells[i][j] = new Square(0, 0, 0, 0);

     // приводим ячейки к виду пустого лабиринта
     clearCells();
}

Maze::~Maze() {
    // Освобождаем выделенную ранее память
    for (int i = 0; i < cells_h; i++)
        for (int j =0; j < cells_v; j++)
            delete cells[i][j];

    for (int i = 0; i < cells_h; i++) {
       delete [] cells[i];
    }
    delete [] cells;

    for (int i = 0; i < cells_h; i++) {
        delete [] alg_Li[i];
    }
    delete [] alg_Li;
}

void Maze::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    //  рисуем фоновый прямоугольник
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->size().width(), this->size().height());

    // рисуем ячейки
    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i < cells_v; i++)
        for (int j = 0; j < cells_h; j++) {
            if ( cells[i][j]->get_left_edge() )
                painter.drawLine(j * width,  i * height, j * width,  (i + 1) * height);
            if ( cells[i][j]->get_top_edge() )
                painter.drawLine(j * width,  i * height, (j + 1) * width, i * height);
            if ( cells[i][j]->get_right_edge() )
                painter.drawLine((j + 1) * width,  i * height, (j + 1) * width, (i + 1) * height);
            if ( cells[i][j]->get_bottom_edge() )
                painter.drawLine(j * width,  (i + 1) * height, (j + 1) * width, (i + 1) * height);
        }

    // если найден путь между точками, рисуем его
    painter.setPen(QPen());
    painter.setBrush(QBrush(Qt::red, Qt::Dense4Pattern));
    if ( path_found )
        for (int i = 0; i < cells_v; i++)
            for (int j = 0; j < cells_h; j++)
                if (alg_Li[i][j] == -3)
                    painter.drawRect(j * width + 4, i * height + 4, 11, 11);

    // рисуем две точки
    painter.setPen(QPen(Qt::green, 1));
    painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    painter.drawEllipse(x1, y1, 15, 15);

    painter.setPen(QPen(Qt::yellow, 1));
    painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    painter.drawEllipse(x2, y2, 15, 15);
}

void Maze::generate() {
    clearCells();
    path_found = false;
    generate(cells_h, cells_v, 0, 0);
    emitGenerationComplete();
    repaint();
}

void Maze::generate(int nH, int nV, int sH, int sV) {

    // 1) если комната в длинну или ширину равна 1 ячейке, то внутри уже нельзя ничего генерировать => выходим
    if ( (nH == 1 || nV == 1) )
        return;

    // 2) если комната 2 х 2, то проводим одну границу внутри произвольно
                                                        //         __ __
                                                        //        |   |	|
                                                        //        |__ __|
    if (nH == 2 && nV == 2) {
        switch (qrand() % 4) {
            case 0: // горизонтальный слева
                    cells[sV][sH]->set_bottom_edge(true);
                break;
            case 1: // горизонтальный справа
                    cells[sV][sH + 1]->set_bottom_edge(true);
                break;
            case 2: // вертикальный сверху
                    cells[sV][sH]->set_right_edge(true);
                break;
            case 3: // вертикальный снизу
                    cells[sV + 1][sH]->set_right_edge(true);
                break;
        }
        return;
    }

    // 3) если одна из сторон комнаты в две ячейки, а другая больше, то проводим лишь одну границу
                                                                                                                        //    __ __ __
                                                                                                                        //   |__ __    |
                                                                                                                        //   |__ __ __|
    if (nH == 2 && nV > 2) {
        // делим комнату пополам
        for (int i = sV; i < sV + nV; i++)
            cells[i][sH]->set_right_edge(true);
        // в произвольном месте ставим дырку
        int tmp1 = qrand() % (nV) + sV;
        cells[tmp1][sH]->set_right_edge(false);
       return;
    }
    if (nH > 2 && nV == 2) {
        // делим комнату пополам
        for (int i = sH; i < sH + nH; i++)
            cells[sV][i]->set_bottom_edge(true);
        // в произвольном месте ставим дырку
        int tmp2 = qrand() % (nH) + sH;
        cells[sV][tmp2]->set_bottom_edge(false);
        return;
    }


    // 4) если все стороны больше чем в две ячейки, продолжаем выполнять всё что ниже

    // находим номера ячеек по которым будут проходить границы
    int indexH = qrand() % (nH - 1);
    int indexV = qrand() % (nV - 1);

    // разбиваем комнату на четыре сектора
    for (int i = 0; i < nV; i++)
        cells[sV + i][indexH + sH]->set_right_edge(true);

    for (int i = 0; i < nH; i++)
        cells[indexV + sV][sH + i]->set_bottom_edge(true);


    // находим номера ячеек на границе в которых делаем отверстия
    int holeH, holeV;

    if ( indexH == 0 ) {
        holeH = 0;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
        holeH = qrand() % (nH - 2) + 1;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
    } else if ( nH - indexH == 2 ) {
        holeH = qrand() % (indexH + 1);
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
        holeH = indexH + 1;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
    } else {
        holeH = qrand() % (indexH + 1);
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
        holeH = qrand() % (nH - indexH - 2) + indexH + 1;
        cells[indexV + sV][holeH + sH]->set_bottom_edge(false);
    }

    if ( indexV == 0 ) {
        holeV = 0;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
        holeV = qrand() % (nV - 2 ) + 1;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
    } else if ( nV - indexV == 2 ) {
        holeV = qrand() % (indexV + 1);
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
        holeV = indexV + 1;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
    } else {
        holeV = qrand() % (indexV + 1);
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
        holeV = qrand() % (nV - indexV - 2 ) + indexV + 1;
        cells[holeV + sV][indexH + sH]->set_right_edge(false);
    }

    // закрываем один из проходов
    switch (qrand() % 4) {
        case 0: // горизонтальный слева
            for (int i = sH; i < sH + indexH + 1; i++)
                cells[indexV + sV][i]->set_bottom_edge(true);
            break;
        case 1: // горизонтальный справа
            for (int i = 0; i < nH - indexH - 1; i++)
                cells[indexV + sV][indexH + 1 + i + sH]->set_bottom_edge(true);
            break;
        case 2: // вертикальный сверху
            for (int i = sV; i < sV + indexV + 1; i++)
                cells[i][indexH + sH]->set_right_edge(true);
            break;
        case 3: // вертикальный снизу
            for (int i =  0; i < nV - indexV - 1; i++)
                cells[indexV + 1 + i + sV][indexH + sH]->set_right_edge(true);
            break;
    }

    // рекурсия в нижний правый отсек
    generate(nH - indexH - 1, nV - indexV - 1, sH + indexH + 1 , sV + indexV + 1);
    // рекурсия в верхний левый отсек
    generate(indexH + 1, indexV + 1, sH, sV);
    // рекурсия в верхний правый отсек
    generate(nH - indexH - 1, indexV + 1, sH + indexH + 1, sV);
    // рекурсия в нижний левый отсек
    generate(indexH + 1, nV - indexV - 1, sH, sV + indexV + 1);
}

void Maze::mousePressEvent(QMouseEvent *e) {
    int tmpX, tmpY;
    switch (e->button()) {
        case 1: // левая кнопка мыши
            tmpX = e->pos().x() - e->pos().x() % cells_h + shiftForPoints;
            tmpY = e->pos().y() - e->pos().y() % cells_v + shiftForPoints;
            if (tmpX == x2 && tmpY == y2) {
                x2 = x1;
                y2 = y1;
                x1 = tmpX;
                y1 = tmpY;
            } else {
                x1 = e->pos().x() - e->pos().x() % cells_h + shiftForPoints;
                y1 = e->pos().y() - e->pos().y() % cells_v + shiftForPoints;
            }
            break;
        case 2: // правая кнопка мыши
            tmpX = e->pos().x() - e->pos().x() % cells_h + shiftForPoints;
            tmpY = e->pos().y() - e->pos().y() % cells_v + shiftForPoints;
            if (tmpX == x1 && tmpY == y1) {
                x1 = x2;
                y1 = y2;
                x2 = tmpX;
                y2 = tmpY;
            } else {
                x2 = e->pos().x() - e->pos().x() % cells_h + shiftForPoints;
                y2 = e->pos().y() - e->pos().y() % cells_v + shiftForPoints;
            }
            break;
        default:
            break;
    }
    repaint();
}

void Maze::clearCells() {
    for (int i = 0; i < cells_v; i++)
       for (int j = 0; j < cells_h; j++) {
           cells[i][j]->set_left_edge(false);
           cells[i][j]->set_top_edge(false);
           cells[i][j]->set_right_edge(false);
           cells[i][j]->set_bottom_edge(false);

           if (i == 0)
               cells[i][j]->set_top_edge(true);
           else if (i == cells_v - 1)
               cells[i][j]->set_bottom_edge(true);

           if (j == 0)
               cells[i][j]->set_left_edge(true);
           else if (j == cells_h - 1)
               cells[i][j]->set_right_edge(true);
        }
}

void Maze::pathfinding() {

    for (int i = 0; i < cells_v; i++)
        for (int j = 0; j < cells_h; j++)
            alg_Li[i][j] = -1; // так помечены все ячейки через которые мы не прошли


    alg_Li[ y1 / height ][ x1 / width ] = 0;    // ячейка првой точки
    alg_Li[ y2 / height ][ x2 / width ] = - 2;  // ячейка второй точки

    int attribute = 0;

    path_found = false;

    // распространение волны
    while ( !path_found ) {

        for (int i = 0; i < cells_v; i++)
            for (int j = 0; j < cells_h; j++)
                if (alg_Li[i][j] == attribute) {
                    // ячейка сверху
                    if (i != 0)
                        if (!cells[i - 1][j]->get_bottom_edge()) {
                            if (alg_Li[i - 1][j] == -1)
                                alg_Li[i - 1][j] = attribute + 1;
                            else if (alg_Li[i - 1][j] == -2) {
                                path_found = true;
                                alg_Li[i - 1][j] = attribute + 1;
                            }
                        }
                    // ячейка слева
                    if (j != 0)
                        if (!cells[i][j - 1]->get_right_edge()) {
                            if (alg_Li[i][j - 1] == -1)
                                alg_Li[i][j - 1] = attribute + 1;
                            else if (alg_Li[i][j - 1] == -2) {
                                path_found = true;
                                alg_Li[i][j - 1] = attribute + 1;
                            }
                        }
                    // ячейка снизу
                    if (i != cells_v - 1)
                        if (!cells[i][j]->get_bottom_edge()) {
                            if (alg_Li[i + 1][j] == -1)
                                alg_Li[i + 1][j] = attribute + 1;
                            else if (alg_Li[i + 1][j] == -2) {
                                path_found = true;
                                alg_Li[i + 1][j] = attribute + 1;
                            }
                        }
                    // ячейка справа
                    if (j != cells_h + 1)
                         if (!cells[i][j]->get_right_edge()) {
                            if (alg_Li[i][j + 1] == -1)
                                alg_Li[i][j + 1] = attribute + 1;
                            else if (alg_Li[i][j + 1] == -2) {
                                path_found = true;
                                alg_Li[i][j + 1] = attribute + 1;
                            }
                        }          
            }
        attribute++;
    }

    // востанавливаем путь: все ячейки обозначющие путь отмечаем индексом "-3"
    int i =  y2 / height;
    int j =  x2 / width;
    alg_Li[ i ][ j ] = -3;

    while (attribute != 0) {

        attribute--;
        // ячейка сверху
        if (i != 0)
            if (!cells[i - 1][j]->get_bottom_edge()) {
                if (alg_Li[i - 1][j] == attribute) {
                    alg_Li[i - 1][j] = -3;
                    i = i - 1;
                    continue;
                }
            }
        // ячейка слева
        if (j != 0)
            if (!cells[i][j - 1]->get_right_edge()) {
                if (alg_Li[i][j - 1] == attribute) {
                    alg_Li[i][j - 1] = -3;
                    j = j - 1;
                    continue;
                }
            }
        // ячейка снизу
        if (i != cells_v - 1)
            if (!cells[i][j]->get_bottom_edge()) {
                if (alg_Li[i + 1][j] == attribute) {
                    alg_Li[i + 1][j] = -3;
                    i = i + 1;
                    continue;
                }
            }
        // ячейка справа
        if (j != cells_h + 1)
            if (!cells[i][j]->get_right_edge()) {
                if (alg_Li[i][j + 1] == attribute) {
                    alg_Li[i][j + 1] = -3;
                    j = j + 1;
                    continue;
                }
            }
    }

    repaint();
}
