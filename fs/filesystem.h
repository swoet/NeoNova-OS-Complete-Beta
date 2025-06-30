#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h> // For size_t
#include <stdint.h> // For uintXX_t types
#include <time.h>   // For time_t

// --- Forward Declarations (Conceptual Opaque Types) ---
// These would be fully defined in a real implementation, likely in separate headers.
typedef struct Volume Volume;
typedef struct Snapshot Snapshot;
typedef struct FileHandle FileHandle; // Represents an open file

// --- Core Structures ---

// Extended File Metadata
typedef struct {
    char name[256]; // File/directory name
    size_t size;    // Size in bytes
    time_t creation_time;
    time_t modification_time;
    time_t access_time;

    uint32_t owner_id;
    uint32_t group_id;
    uint16_t permissions; // e.g., rwxrwxrwx represented numerically

    uint8_t is_directory; // Boolean flag
    uint8_t is_symlink;   // Boolean flag
    // uint8_t is_encrypted; // Boolean flag

    // For CoW and Integrity
    uint8_t  is_cow_enabled; // If Copy-on-Write is active for this file/directory
    uint64_t data_checksum;  // Checksum of file content (if applicable)
    uint64_t metadata_checksum; // Checksum of this metadata struct (or parts of it)

    // For future use: compression info, extended attributes pointer, etc.
    // void* extended_attributes;
} FileMetadata;


// Structure representing a storage volume (conceptual)
// A real Volume struct would contain much more: mount point, device ID, block size, etc.
struct Volume {
    char volume_id[64];     // Unique ID for the volume
    char label[128];        // User-friendly label
    size_t total_space;
    size_t free_space;
    // FileSystemType type; // e.g., FS_TYPE_NEOFS, FS_TYPE_EXT4 (conceptual)
    // Snapshot* active_snapshots[MAX_SNAPSHOTS_PER_VOLUME];
    // int snapshot_count;
};

// Structure representing a snapshot (conceptual)
struct Snapshot {
    char snapshot_id[64];   // Unique ID for the snapshot
    char name[128];         // User-friendly name for the snapshot
    time_t creation_time;
    Volume* source_volume;  // Volume this snapshot belongs to
    // size_t size_on_disk; // Space consumed by this snapshot (due to CoW diffs)
    // Snapshot* parent_snapshot; // For chained snapshots
};


// --- API Signatures for Filesystem Operations ---

// Volume Management (Conceptual)
// Volume* Filesystem_MountVolume(const char* device_path, const char* mount_point, const char* fs_type);
// int Filesystem_UnmountVolume(Volume* vol);
// int Filesystem_FormatVolume(const char* device_path, const char* label, const char* fs_type);
// FileMetadata* Filesystem_GetVolumeMetadata(Volume* vol);


// Snapshot Operations
Snapshot* Volume_CreateSnapshot(Volume* vol, const char* snapshot_name);
int Volume_DeleteSnapshot(Volume* vol, Snapshot* snapshot);
int Volume_RevertToSnapshot(Volume* vol, Snapshot* snapshot); // Potentially destructive operation
// Snapshot** Volume_ListSnapshots(Volume* vol, int* count); // Returns array of snapshot pointers


// File Operations (Conceptual - extending standard file ops)
// FileHandle* File_Open(Volume* vol, const char* path, const char* mode);
// int File_Close(FileHandle* fh);
// size_t File_Read(FileHandle* fh, void* buffer, size_t size);
// size_t File_Write(FileHandle* fh, const void* buffer, size_t size);

// CoW-specific conceptual read (might not be a distinct API in reality, but for illustration)
size_t File_ReadCoW(FileHandle* fh, void* buffer, size_t offset, size_t size);
// CoW write is typically transparent: standard File_Write on a CoW-enabled file/volume handles it.

// File Metadata & Integrity
FileMetadata* File_GetMetadata(FileHandle* fh); // Or File_GetMetadataByPath(Volume* vol, const char* path)
// int File_SetMetadata(FileHandle* fh, const FileMetadata* metadata); // For changing owner, perms etc.
uint64_t File_CalculateChecksum(FileHandle* fh); // Calculates checksum for current content
int File_VerifyChecksum(FileHandle* fh); // Compares current content checksum with stored FileMetadata.data_checksum

// Filesystem Integrity Check
// This would be a complex operation, potentially involving checking all metadata, checksums, tree structures.
int Filesystem_CheckIntegrity(Volume* vol, int repair_level); // repair_level could be 0 (check only) to N (attempt aggressive repair)
// int Filesystem_SelfHeal(Volume* vol, FilePath* corrupted_file_path); // Conceptual


// Deduplication (Highly conceptual - often an FS-wide background process)
// int Filesystem_EnableDeduplication(Volume* vol, bool enable);
// DeduplicationStats Filesystem_GetDeduplicationStats(Volume* vol);


// Time Machine-like Backup & Versioning (Conceptual API - would build upon snapshots)
// BackupJob* Backup_StartSystemBackup(Volume* source_vol, Volume* target_vol, BackupPolicy policy);
// int Backup_RestoreFile(const char* file_path, time_t version_timestamp);
// VersionInfo* File_ListVersions(const char* file_path);


#endif // FILESYSTEM_H
