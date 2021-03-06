#include "stdafx.h"
#include <iostream>
#include <limits> // Valida entrada por teclado.
#include <cctype> // toupper();
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include "Entidad.h"

using dbms::Entidad;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using dbms::Type;
using dbms::Datatype;

const std::string SYSTABLE = "Systable.txt";

void clear_screen()
{
#ifdef _WIN64 || _WIN32
	std::system("cls");
#else
	std::system("clear");
#endif
}

enum class MenuTabla {
	EXIT, CREATE, MOSTRAR, DROP, INSERT, SELECT
};
MenuTabla mostrarMenu();

template <class T>
void validarNumero(T& numero, string mensaje);

const std::string CATALOGO_CAMPOS = "entidad_id \t|\t entidad_nombre \t|\t entidad_fecha_creada \r\n";
void crearCatalogo();
void mostrarEntidades(std::vector <Entidad> entidades);

void crearEntidad(int& contador);
void verData(std::vector<Entidad> entidades);
void buscarData(std::vector<Entidad> entidades);
void alta(std::vector<Entidad> entidades);

int main()
{
	crearCatalogo();
	std::vector<Entidad> entidades = std::vector<Entidad>();

	bool haTerminado = false;
	while (!haTerminado) {
		try {

			entidades = Entidad(1, "a").readAll();
			int lastId = entidades.size();
			MenuTabla opcion = mostrarMenu();
			switch (opcion) {
			case MenuTabla::CREATE:
				crearEntidad(lastId);
				break;
			case MenuTabla::MOSTRAR:
				mostrarEntidades(entidades);
				break;
			case MenuTabla::INSERT:
				alta(entidades);
				break;
			case MenuTabla::DROP:
				// Inserte funcion aqui.
				break;
			case MenuTabla::SELECT:
				verData(entidades);
				break;
			default:
				haTerminado = true;
			}
		}
		catch (std::exception& e) {
			cout << e.what();
		}
		//----------------------------------------------
		cout << "\n\n";
		cout << "Presione ENTER para continuar...";
		cin.ignore();
		cin.get();
		cout << "\n\n";
		clear_screen();

	}

}

MenuTabla mostrarMenu() {
	unsigned short opcion;
	const unsigned short ULTIMA = 3;
	bool esValida = false;
	string menu = "\t\tMenu de entidad: \n";
	menu.append("1. Crear una entidad.\n");
	menu.append("2. Mostrar entidades.\n");
	menu.append("3. Eliminar una entidad.\n");
	menu.append("4. Agregar data a una entidad.\n");
	menu.append("5. Ver data a una entidad.\n");
	menu.append("0. Salir.\n\n");

	while (!esValida) {
		validarNumero(opcion, menu);
		esValida = (opcion == static_cast<unsigned short>(MenuTabla::EXIT)) || (opcion <= ULTIMA);
	}

	return static_cast<MenuTabla>(opcion);
}

template <class T>
void validarNumero(T& numero, string mensaje) {
	bool esValido = false;
	while (!esValido) {
		cout << mensaje;
		if (cin >> numero) {
			esValido = true;
		}
		else {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		cout << "\n\n";
	}

}

void crearCatalogo() {
	if (!std::experimental::filesystem::exists(SYSTABLE)) {
		std::ofstream archivo;
		archivo.open(SYSTABLE, std::ios::out);
		if (!archivo.fail()) {
			archivo << CATALOGO_CAMPOS;
		}

		archivo.close();
	}
}

void mostrarEntidades(std::vector<Entidad> entidades)
{
	cout << CATALOGO_CAMPOS;
	for (auto entidad : entidades)
	{
		cout << entidad.getId() << "\t|\t" << entidad.getNombre() << "\t|\t" << entidad.getFecha() << "\n";
	}
	cout << "\n";

}

void crearEntidad(int& contador) {
	string nombre;
	int tipoDato;
	cout << "Nombre de la entidad: ";
	cin >> nombre;

	auto entidad = std::unique_ptr<Entidad>(new Entidad(contador + 1, nombre));

	bool haTerminado = false;
	while (!haTerminado)
	{
		string columnaNombre, mensaje;
		cout << "Nombre de la columna: ";
		cin >> columnaNombre;

		mensaje.append("\nSeleccione tipo de dato\n");
		Datatype datatype;
		for (int i = static_cast<int>(dbms::Type::SMALLINT); i <= Datatype::getSize(); ++i) {
			datatype = dbms::tipos->operator[](static_cast<Type>(i));
			mensaje.append(std::to_string(i) + ". " + datatype.toString() + "\n");
		}
		validarNumero(tipoDato, mensaje);

		datatype = dbms::tipos->operator[](static_cast<Type>(tipoDato));
		switch (datatype.getType())
		{
		case Type::CHAR:
		case Type::VARCHAR:
		case Type::NUMERIC:
		{
			std::string mensaje = "Ingrese longitud del " + datatype.toString() + ": ";
			short length = 0;
			validarNumero(length, mensaje);
			datatype.setLength(length);

			if (datatype.getType() == Type::NUMERIC) {
				short precision = 0;
				mensaje.clear();
				mensaje.append("Ingrese precision del NUMERIC: ");
				validarNumero(precision, mensaje);
				datatype.setPrecision(precision);
			}
		}
		default:
			break;
		}

		entidad->agregarColumna(columnaNombre, datatype);


		char opcion;
		cout << "Desea agregar otra columna? [S/N]\n";
		cin >> opcion;
		opcion = std::toupper(opcion);
		haTerminado = (opcion == 'N');
	}
}

void verData(std::vector<Entidad> entidades) {
	mostrarEntidades(entidades);

	int id;
	cout << "Ingrese id de la entidad a consultar: ";
	cin >> id;

	Entidad entidad = entidades[id - 1];

	auto registros = entidad.getData();

	cout << "Numero de registros: " << registros.size() << "\n\n";
	for (int i = 1; i < registros.size(); i++) {
		cout << registros.find(i)->first;
	}
}

void buscarData(std::vector<Entidad> entidades) {
	mostrarEntidades(entidades);

	int id;
	cout << "Ingrese id de la entidad a consultar: ";
	cin >> id;

	Entidad entidad = entidades[id - 1];

	cout << "Ingrese id del registro a buscar: ";
	cin >> id;

	auto map = entidad.getData();
	auto registro = map.find(id);

	if (registro != map.end()) {
		cout << registro->first << "\n";
	}
	else
	{
		cout << "El registro no pudo ser encontrado.\n\n";
	}

}

void alta(std::vector<Entidad> entidades) {

	mostrarEntidades(entidades);
	int id;
	cout << "Ingrese id de la entidad: ";
	cin >> id;

	Entidad entidad = entidades[id - 1];

	bool haTerminado = false;
	while (!haTerminado)
	{
		std::vector<string> data;
		for (auto columna : entidad.getColumnas())
		{
			string dato;
			cout << columna.getNombre() << ": ";
			cin >> dato;
			data.push_back(dato);
		}

		entidad.alta(data);

		cout << "\n\n";
		char opcion;
		cout << "Desea insertar otro registro? [S/N] ";
		cin >> opcion;
		opcion = toupper(opcion);

		haTerminado = (opcion == 'S');
	}

}