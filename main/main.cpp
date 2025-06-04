#include <iostream>
#include <variant>
#include <string>

using namespace std;

const double EPS = 1E-15; // для сравнения double с 0

struct Error {
    string message;
};

class Vector3D {
public:
    double X, Y, Z;
    Vector3D(double _X, double _Y, double _Z) : X(_X), Y(_Y), Z(_Z) {}
    Vector3D operator-(const Vector3D& point) { return Vector3D(X - point.X, Y - point.Y, Z - point.Z); }
    Vector3D operator+(const Vector3D& point) { return Vector3D(X + point.X, Y + point.Y, Z + point.Z); }
    Vector3D operator*(double scale) { return Vector3D(X * scale, Y * scale, Z * scale); }
    Vector3D vec_mult(Vector3D& vec) { return Vector3D(Y * vec.Z - vec.Y * Z, Z * vec.X - X * vec.Z, X * vec.Y - Y * vec.X); } // векторное произведение
    double scal_mult(Vector3D& vec) { return X * vec.X + Y * vec.Y + Z * vec.Z; } // скалярное произведение
    double norm() { return sqrt(scal_mult(*this)); } // норма
};

class Segment3D {
public:
    Vector3D start, end;
    Segment3D(Vector3D _start, Vector3D _end) : start(_start), end(_end) {}
    Vector3D dir_vec() { return end - start; } // направляющий вектор
};

double Det(Vector3D& a1, Vector3D& a2, Vector3D& a3) { // определитель для матрицы, состоящей из строк Vector3D в заданном порядке и столбцов X, Y, Z
    return a1.X * a2.Y * a3.Z + a2.X * a3.Y * a1.Z + a3.X * a1.Y * a2.Z -
        a3.X * a2.Y * a1.Z - a1.X * a3.Y * a2.Z - a2.X * a1.Y * a3.Z;
}

variant<Vector3D, Error> Intersect(Segment3D& segment1, Segment3D& segment2) {
    Vector3D dir_vec1 = segment1.dir_vec(), dir_vec2 = segment2.dir_vec();
    Vector3D diff_start = segment2.start - segment1.start;
    double det = Det(diff_start, dir_vec1, dir_vec2);

    if (fabs(det) > EPS) {
        return Error("прямые находятся не в одной плоскости!");
    }
    else {
        Vector3D n = dir_vec1.vec_mult(dir_vec2);
        double norm_vec = n.norm();
        if (norm_vec < EPS) {
            return Error("прямые коллинеарны!");
        }

        double _det = dir_vec2.X * dir_vec1.Y - dir_vec1.X * dir_vec2.Y;
        double t;

        if (fabs(_det) > EPS) {
            t = (dir_vec2.X * diff_start.Y - dir_vec2.Y * diff_start.X) / _det;
        }
        else {
            _det = dir_vec2.X * dir_vec1.Z - dir_vec1.X * dir_vec2.Z;
            t = (dir_vec2.X * diff_start.Z - dir_vec2.Z * diff_start.X) / _det;
        }

        if (t < -EPS || t > 1 + EPS) {
            return Error("пересекаются продолжения, а не сами отрезки!");;
        }

        Vector3D res = segment1.start + dir_vec1 * t;
        return res;
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");
    Vector3D A1(0., 0., 0.), B1(3., 0., 2.);
    Vector3D A2(0., 0., 2.), B2(3., 0., 0.);
    Segment3D segment1(A1, B1), segment2(A2, B2);
    auto res = Intersect(segment1, segment2);

    if (holds_alternative<Vector3D>(res)) {
        Vector3D point = get<Vector3D>(res);
        cout << "Точка пересечения: " << endl << "x: " << point.X << ", y: " << point.Y << ", z: " << point.Z << endl;
    }
    else {
        Error err = get<Error>(res);
        cout << "Невозможно найти точку пересечения: " << err.message << endl;
    }
}
