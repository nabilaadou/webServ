#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <string>
#include <pwd.h>
#include <grp.h>
#include <string.h>

// Function to get file type
std::string getFileType(mode_t mode) {
    if (S_ISREG(mode)) return "Regular File";
    if (S_ISDIR(mode)) return "Directory";
    if (S_ISLNK(mode)) return "Symbolic Link";
    if (S_ISFIFO(mode)) return "FIFO/Named Pipe";
    if (S_ISCHR(mode)) return "Character Device";
    if (S_ISBLK(mode)) return "Block Device";
    if (S_ISSOCK(mode)) return "Socket";
    return "Unknown";
}

// Function to get file permissions string (like ls -l)
std::string getPermissionString(mode_t mode) {
    std::string perms;
    
    // User permissions
    perms += (mode & S_IRUSR) ? "r" : "-";
    perms += (mode & S_IWUSR) ? "w" : "-";
    perms += (mode & S_IXUSR) ? "x" : "-";
    
    // Group permissions
    perms += (mode & S_IRGRP) ? "r" : "-";
    perms += (mode & S_IWGRP) ? "w" : "-";
    perms += (mode & S_IXGRP) ? "x" : "-";
    
    // Others permissions
    perms += (mode & S_IROTH) ? "r" : "-";
    perms += (mode & S_IWOTH) ? "w" : "-";
    perms += (mode & S_IXOTH) ? "x" : "-";
    
    return perms;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    struct stat file_stat;
    
    // Try to get file statistics
    if (stat(argv[1], &file_stat) != 0) {
        std::cerr << "Error getting file stats: " << strerror(errno) << std::endl;
        return 1;
    }

    // Print file information
    std::cout << "\n=== File Information for: " << argv[1] << " ===\n" << std::endl;

    // Basic file information
    std::cout << "File Type: " << getFileType(file_stat.st_mode) << std::endl;
    std::cout << "File Permissions: " << getPermissionString(file_stat.st_mode) << std::endl;
    
    // Size information
    std::cout << "File Size: " << file_stat.st_size << " bytes" << std::endl;
    std::cout << "Block Size: " << file_stat.st_blksize << " bytes" << std::endl;
    std::cout << "Number of Blocks: " << file_stat.st_blocks << std::endl;

    // Time information
    std::cout << "\nTime Information:" << std::endl;
    std::cout << "Last Access Time: " << ctime(&file_stat.st_atime);
    std::cout << "Last Modification Time: " << ctime(&file_stat.st_mtime);
    std::cout << "Last Status Change: " << ctime(&file_stat.st_ctime);

    // Owner information
    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    
    std::cout << "\nOwnership Information:" << std::endl;
    std::cout << "Owner User ID: " << file_stat.st_uid;
    if (pw != nullptr) std::cout << " (" << pw->pw_name << ")";
    std::cout << std::endl;
    
    std::cout << "Owner Group ID: " << file_stat.st_gid;
    if (gr != nullptr) std::cout << " (" << gr->gr_name << ")";
    std::cout << std::endl;

    // Device information
    std::cout << "\nDevice Information:" << std::endl;
    std::cout << "Device ID: " << file_stat.st_dev << std::endl;
    if (S_ISCHR(file_stat.st_mode) || S_ISBLK(file_stat.st_mode)) {
        std::cout << "Special Device ID: " << file_stat.st_rdev << std::endl;
    }

    // Inode information
    std::cout << "\nInode Information:" << std::endl;
    std::cout << "Inode Number: " << file_stat.st_ino << std::endl;
    std::cout << "Number of Hard Links: " << file_stat.st_nlink << std::endl;

    return 0;
}
