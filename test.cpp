#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

int main(void)
 {
    // Create temporary directory
    if (mkdir("./tmp", 0755) == -1 && errno != EEXIST) {
        perror("Error creating directory");
        return EXIT_FAILURE;
    }

    // Write phase
    FILE* tmpFileWrite = fopen("./tmp/CgiDataUpload", "w");
    if (!tmpFileWrite) {
        perror("Error opening file for writing");
        return EXIT_FAILURE;
    }

    const char* data = "Test data content\n";
    if (fwrite(data, 1, strlen(data), tmpFileWrite) != strlen(data)) {
        perror("Error writing to file");
        fclose(tmpFileWrite);
        return EXIT_FAILURE;
    }
    
    std::cout << "Successfully wrote to file" << std::endl;
    fclose(tmpFileWrite);

    // Read phase
    FILE* tmpFileRead = fopen("./tmp/CgiDataUpload", "r");
    if (!tmpFileRead) {
        perror("Error opening file for reading");
        return EXIT_FAILURE;
    }

    // Verify content
    char buffer[256];
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), tmpFileRead);
    fclose(tmpFileRead);

    if (bytesRead != strlen(data)) {
        std::cerr << "Data mismatch! Expected " << strlen(data)
                  << " bytes, got " << bytesRead << " bytes\n";
        return EXIT_FAILURE;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Read content:\n" << buffer << std::endl;

    // Cleanup
    if (remove("./tmp/CgiDataUpload") != 0) {
        perror("Error deleting file");
    }
    if (rmdir("./tmp") != 0 && errno != ENOTEMPTY) {
        perror("Error removing directory");
    }

    return EXIT_SUCCESS;
}
