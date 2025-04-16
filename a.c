typedef struct {
    int l_count;
    float *l_data; // floats (vec3, vec2, etc.)
} g_accessor_data_t;

typedef struct {
    char *l_name;
    int l_accessor_position;
    int l_accessor_normal;
    int l_accessor_texcoord0;
    int l_accessor_indices;
} g_mesh_primitive_t;

typedef struct {
    int l_primitive_count;
    g_mesh_primitive_t *l_primitives;
} g_mesh_t;

typedef struct {
    char *l_uri; // e.g., "scene.bin"
    int l_byte_length;
} g_buffer_t;

typedef struct {
    int l_buffer;      // index into g_buffer
    int l_byte_offset;
    int l_byte_length;
} g_buffer_view_t;

typedef struct {
    int l_buffer_view;
    int l_component_type; // GLTF constant (e.g., 5126 = float)
    int l_count;
    char *l_type;         // "VEC3", "SCALAR", etc.
} g_accessor_t;

typedef struct {
    g_buffer_t *l_buffers;
    int l_buffer_count;

    g_buffer_view_t *l_buffer_views;
    int l_buffer_view_count;

    g_accessor_t *l_accessors;
    int l_accessor_count;

    g_mesh_t *l_meshes;
    int l_mesh_count;
} g_gltf_t;

