//! \addtogroup World
//! \{
#ifndef CETECH_CAMERA_H
#define CETECH_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Includes
//==============================================================================

#include <stddef.h>

//==============================================================================
// Typedefs
//==============================================================================

typedef struct world_s world_t;
typedef struct entity_s entity_t;
typedef struct mat44f_s mat44f_t;

//! Camera struct
typedef struct camera_s {
    uint32_t idx;
} camera_t;



//==============================================================================
// Api
//==============================================================================

//! Camera API V0
struct camera_api_v0 {

    //! Has entity camera component?
    //! \param world World
    //! \param entity Entity
    //! \return 1 if has else 0
    int (*has)(world_t world,
               entity_t entity);

    //! Is camera valid?
    //! \param camera Camera
    //! \return 1 if is oK else 0
    int (*is_valid)(camera_t camera);


    //! Get camera project and view matrix
    //! \param world World
    //! \param camera Camera
    //! \param proj Project matrix
    //! \param view View Matrix
    void (*get_project_view)(world_t world,
                             camera_t camera,
                             mat44f_t *proj,
                             mat44f_t *view);


    //! Get camera component
    //! \param world World
    //! \param entity Entity
    //! \return Camera component
    camera_t (*get)(world_t world,
                    entity_t entity);

    //! Create camera
    //! \param world World
    //! \param entity Entity
    //! \param near Near
    //! \param far Far
    //! \param fov Fov
    //! \return New camera
    camera_t (*create)(world_t world,
                       entity_t entity,
                       float near,
                       float far,
                       float fov);
};

#ifdef __cplusplus
}
#endif

#endif //CETECH_CAMERA_H

//! |}