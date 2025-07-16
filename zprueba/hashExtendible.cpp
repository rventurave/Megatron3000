#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <type_traits> // Required for std::hash
#include <iomanip>     // Required for std::setw
#include <bitset>      // Required for std::bitset

// Forward declaration for Bucket, needed for friend declaration if T requires it
template <typename T>
class HashExtendido;

// --- Bucket Class ---
template <typename T>
class Bucket {
public:
    int capacidad;
    int profundidadLocal;
    std::vector<T> elementos;

    // Constructor
    Bucket(int cap, int prof) : capacidad(cap), profundidadLocal(prof) {}

    // Checks if the bucket is full
    bool lleno() const {
        return elementos.size() >= capacidad;
    }

    // Adds a value to the bucket if there's space
    bool agregar(const T& valor) {
        if (!lleno()) {
            elementos.push_back(valor);
            return true;
        }
        return false;
    }

    // Checks if the bucket contains a specific value
    bool contiene(const T& valor) const {
        for (const auto& e : elementos) {
            if (e == valor) return true;
        }
        return false;
    }

    // Removes a value from the bucket
    // Returns true if the value was found and removed, false otherwise
    bool eliminar(const T& valor) {
        for (size_t i = 0; i < elementos.size(); ++i) {
            if (elementos[i] == valor) {
                elementos.erase(elementos.begin() + i);
                return true; // Element found and removed
            }
        }
        return false; // Element not found
    }

    // Displays the contents of the bucket
    void mostrar() const {
        std::cout << "Bucket(prof=" << profundidadLocal << ", elementos={";
        for (size_t i = 0; i < elementos.size(); ++i) {
            std::cout << elementos[i];
            if (i + 1 < elementos.size()) std::cout << ", ";
        }
        std::cout << "})";
    }
};

// --- HashExtendido Class ---
template <typename T>
class HashExtendido {
private:
    int capacidadBucket;
    int profundidadGlobal;
    std::vector<std::shared_ptr<Bucket<T>>> directorio; // Directory of shared pointers to buckets

    // Computes the hash for a given value
    size_t obtenerHash(const T& valor) const {
        return std::hash<T>{}(valor);
    }

    // Determines the directory index based on the global depth
    int obtenerIndice(size_t hash) const {
        // Uses the least significant 'profundidadGlobal' bits of the hash
        return hash & ((1 << profundidadGlobal) - 1);
    }

    // Handles the splitting of a bucket when it overflows
    void dividirBucket(int indice) {
        auto viejo = directorio[indice]; // The bucket that needs to be split
        int profAnt = viejo->profundidadLocal;
        viejo->profundidadLocal++; // Increment local depth of the original bucket (now represents one half)

        // Create two new buckets for the split
        auto b0 = std::make_shared<Bucket<T>>(capacidadBucket, viejo->profundidadLocal);
        auto b1 = std::make_shared<Bucket<T>>(capacidadBucket, viejo->profundidadLocal);

        // Redistribute elements from the old bucket into the new ones
        for (const auto& val : viejo->elementos) {
            // Check the (profAnt)-th bit of the hash to determine which new bucket it goes to
            int bit = (obtenerHash(val) >> profAnt) & 1;
            if (bit == 0)
                b0->agregar(val);
            else
                b1->agregar(val);
        }

        // Clear elements from the old bucket after redistributing them
        // The old 'viejo' shared_ptr might still be pointed to by other directory entries,
        // but its elements are now empty. This isn't strictly necessary as the pointers
        // will be updated below.
        viejo->elementos.clear();

        
        for (int i = 0; i < directorio.size(); ++i) {

            if (directorio[i] == viejo) {
                int bit = (i >> profAnt) & 1; // Check the relevant bit of the directory index
                directorio[i] = (bit == 0) ? b0 : b1;
            }
        }
    }

    void fusionarBuckets(int indice) {
        auto bucketActual = directorio[indice];

        if (bucketActual->elementos.empty() && bucketActual->profundidadLocal > 1) {
            int profLocal = bucketActual->profundidadLocal;
            // Calculate the buddy index by flipping the (profLocal - 1)-th bit
            int buddyIndex = indice ^ (1 << (profLocal - 1));
            auto buddyBucket = directorio[buddyIndex];

            if (buddyBucket && buddyBucket->profundidadLocal == profLocal) {
                // Determine the common prefix length (the depth before they split)
                int commonPrefixLength = profLocal - 1;
                int commonPrefix = indice & ((1 << commonPrefixLength) - 1);

                // Re-point all directory entries that point to either the `bucketActual`
                // or `buddyBucket` to now point to the `buddyBucket`.
                // This effectively "merges" the empty bucket's entries into the buddy.
                // It iterates over all possible directory indices that would share the `commonPrefix`.
                for (int i = 0; i < (1 << (profundidadGlobal - commonPrefixLength)); ++i) {
                    int dirIndex = (commonPrefix | (i << commonPrefixLength));
                    // If this directory entry points to either of the buckets in the pair,
                    // make it point to the buddy.
                    if (directorio[dirIndex] == bucketActual || directorio[dirIndex] == buddyBucket) {
                        directorio[dirIndex] = buddyBucket;
                    }
                }
                // Decrement the local depth of the now-merged bucket (the buddy)
                buddyBucket->profundidadLocal = commonPrefixLength;
                std::cout << "DEBUG: Cubeta en indice " << indice << " fusionada con su cubeta 'hermana' en " << buddyIndex << ". Nueva profundidad local: " << buddyBucket->profundidadLocal << std::endl;

                // TODO: Add logic here to potentially reduce global depth if all buddies
                // at the current global depth can be merged and the overall directory size
                // can be halved. This is a more complex step.
            }
        }
    }

public:
    // Constructor for HashExtendido
    HashExtendido(int cap) : capacidadBucket(cap), profundidadGlobal(1) {
        directorio.resize(1 << profundidadGlobal); // Initial directory size 2^1 = 2
        // Initialize all directory entries to point to the same initial bucket
        for (int i = 0; i < (1 << profundidadGlobal); ++i)
            directorio[i] = std::make_shared<Bucket<T>>(capacidadBucket, profundidadGlobal);
    }

    // Inserts a value into the hash table
    void insertar(const T& valor) {
        while (true) {
            size_t h = obtenerHash(valor);
            int indice = obtenerIndice(h);
            auto bucket = directorio[indice];

            // Optional: Prevent duplicate insertions
            if (bucket->contiene(valor)) {
                std::cout << "Valor " << valor << " ya existe en el hash. No se inserta duplicado." << std::endl;
                return;
            }

            if (bucket->lleno()) {
                // If the bucket is full and its local depth equals global depth,
                // we need to double the directory size.
                if (bucket->profundidadLocal == profundidadGlobal) {
                    int tam = directorio.size();
                    profundidadGlobal++;
                    directorio.resize(1 << profundidadGlobal); // Double the directory size
                    // Copy existing pointers to the new half of the directory
                    for (int i = 0; i < tam; ++i) {
                        directorio[i + tam] = directorio[i];
                    }
                }
                // Split the overflowing bucket
                dividirBucket(indice);
            } else {
                // If there's space, add the value to the bucket
                bucket->agregar(valor);
                std::cout << "Valor " << valor << " insertado exitosamente." << std::endl;
                break; // Exit loop after successful insertion
            }
        }
    }

    // Searches for a value in the hash table
    // Returns true if found, false otherwise
    bool buscar(const T& valor) const {
        size_t h = obtenerHash(valor);
        int indice = obtenerIndice(h);
        return directorio[indice]->contiene(valor);
    }

    // Deletes a value from the hash table
    // Returns true if the value was found and removed, false otherwise
    bool eliminar(const T& valor) {
        size_t h = obtenerHash(valor);
        int indice = obtenerIndice(h);
        auto bucket = directorio[indice];

        if (bucket->eliminar(valor)) {
            std::cout << "Valor " << valor << " eliminado exitosamente." << std::endl;
            // After deletion, check if the bucket is empty and if we can merge
            if (bucket->elementos.empty()) { // Simple check for emptiness for merging
                fusionarBuckets(indice);
            }
            return true;
        } else {
            std::cout << "Valor " << valor << " no encontrado para eliminar." << std::endl;
            return false;
        }
    }

    // Displays the current state of the hash table
    void mostrar() const {
        std::cout << "\n--- Estado del Hash ---\n";
        std::cout << "Profundidad Global: " << profundidadGlobal << "\n";
        std::cout << "Tamano del Directorio: " << directorio.size() << "\n";

        // Use a map to store unique bucket pointers to avoid displaying the same bucket multiple times
        std::map<void*, std::shared_ptr<Bucket<T>>> unicos;
        for (int i = 0; i < directorio.size(); ++i) {
            // Display directory index in binary format (padded with leading zeros if necessary)
            std::cout << "   [" << std::setw(3) << std::bitset<8>(i).to_string().substr(8 - profundidadGlobal) << "] -> ";
            directorio[i]->mostrar(); // Display the bucket contents
            std::cout << "\n";
            unicos[directorio[i].get()] = directorio[i]; // Store unique bucket pointers
        }

        std::cout << "\nBuckets Unicos:\n";
        for (const auto& [_, b] : unicos) {
            std::cout << "   ";
            b->mostrar();
            std::cout << "\n";
        }
        std::cout << "------------------------\n";
    }
};

// --- Main Function ---
int main() {
    int capacidad;
    std::cout << "Ingrese tamano del bucket: ";
    std::cin >> capacidad;

    // Create an Extendible Hash table for integers
    HashExtendido<int> hash(capacidad);

    int opcion, valor;
    do {
        std::cout << "\n--- MENU ---\n";
        std::cout << "1. Insertar\n";
        std::cout << "2. Buscar\n";
        std::cout << "3. Mostrar\n";
        std::cout << "4. Eliminar\n"; // New option for deletion
        std::cout << "5. Salir\n";
        std::cout << "Opcion: ";
        std::cin >> opcion;

        switch (opcion) {
        case 1:
            std::cout << "Valor a insertar: ";
            std::cin >> valor;
            hash.insertar(valor);
            break;
        case 2:
            std::cout << "Valor a buscar: ";
            std::cin >> valor;
            std::cout << (hash.buscar(valor) ? "El valor SI esta en el hash.\n" : "El valor NO esta en el hash.\n");
            break;
        case 3:
            hash.mostrar();
            break;
        case 4: // Handle the new delete option
            std::cout << "Valor a eliminar: ";
            std::cin >> valor;
            hash.eliminar(valor);
            break;
        case 5:
            std::cout << "Saliendo del programa. ¡Hasta luego!\n";
            break;
        default:
            std::cout << "Opcion no valida. Por favor, intente de nuevo.\n";
        }
    } while (opcion != 5); // Loop until the user chooses to exit (option 5)

    return 0;
}