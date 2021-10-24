/*
 * rml.cpp
 *
 *  Created on: 30 мая 2021 г.
 *      Author: alex9932
 */

#include "rml.h"

#include "fml1.h"
#include "fml2.h"
#include "rg_modelloader.h"

void rml_fml1(rg_string input, rg_string output) {
	FMLModelData* data = FML1_o_fml_load(input, FML_FULL_MODEL);
	printf("Total vertices: %d\n", data->vertices_size);

	printf("Generating mesh...\n");
	rg_mesh_t* mesh = rg_newMesh();

	printf("Converting\n");
	mesh->vertex_count = data->vertices_size / 3;
	mesh->index_count = data->indices_size;
	mesh->indices = data->indices_ptr;
	mesh->vertices = (rg_vertex_t*)malloc(sizeof(rg_vertex_t) * mesh->vertex_count);
	for(Uint32 i = 0; i < mesh->vertex_count; i++) {
		mesh->vertices[i].vertex.x = data->vertices_ptr[i*3 + 0];
		mesh->vertices[i].vertex.y = data->vertices_ptr[i*3 + 1];
		mesh->vertices[i].vertex.z = data->vertices_ptr[i*3 + 2];
		mesh->vertices[i].normal.x = data->normals_ptr[i*3 + 0];
		mesh->vertices[i].normal.y = data->normals_ptr[i*3 + 1];
		mesh->vertices[i].normal.z = data->normals_ptr[i*3 + 2];
		mesh->vertices[i].tangent.x = data->tangents_ptr[i*3 + 0];
		mesh->vertices[i].tangent.y = data->tangents_ptr[i*3 + 1];
		mesh->vertices[i].tangent.z = data->tangents_ptr[i*3 + 2];
		mesh->vertices[i].t_coord.x = data->texcoords_ptr[i*2 + 0];
		mesh->vertices[i].t_coord.y = data->texcoords_ptr[i*2 + 1];
	}

	printf("Writing rgm file\n");
	rg_rmlWrite(output, mesh);

	free(mesh->vertices);
	rg_freeMesh(mesh);
	FML1_model_free_modeldata(data);

	printf("Done!\n");
}

void rml_fml2(rg_string input, rg_string output) {
	FILE* file = fopen(input, "rb");
	if(!file) {
		printf("%s not found!\n", input);
		return;
	}
	fseek(file, 0, SEEK_END);
	long int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	void* data = malloc(length);
	fread(data, 1, length, file);
	fclose(file);
	Model* model = FML2_fml_unpack(data);
	printf("Total vertices: %d\n", model->vertex_count);

	printf("Generating mesh...\n");
	rg_mesh_t* mesh = rg_newMesh();

	printf("Converting\n");
	mesh->vertex_count = model->vertex_count / 3;
	mesh->index_count = model->index_count;
	mesh->indices = model->indices;
	mesh->vertices = (rg_vertex_t*)malloc(sizeof(rg_vertex_t) * mesh->vertex_count);
	for(Uint32 i = 0; i < mesh->vertex_count; i++) {
		mesh->vertices[i].vertex.x = model->vertices[i*3 + 0];
		mesh->vertices[i].vertex.y = model->vertices[i*3 + 1];
		mesh->vertices[i].vertex.z = model->vertices[i*3 + 2];
		mesh->vertices[i].normal.x = model->normals[i*3 + 0];
		mesh->vertices[i].normal.y = model->normals[i*3 + 1];
		mesh->vertices[i].normal.z = model->normals[i*3 + 2];
		mesh->vertices[i].tangent.x = model->tangents[i*3 + 0];
		mesh->vertices[i].tangent.y = model->tangents[i*3 + 1];
		mesh->vertices[i].tangent.z = model->tangents[i*3 + 2];
		mesh->vertices[i].t_coord.x = model->tex_coords[i*2 + 0];
		mesh->vertices[i].t_coord.y = model->tex_coords[i*2 + 1];
	}

	printf("Writing rgm file\n");
	rg_rmlWrite(output, mesh);
	free(mesh->vertices);
	rg_freeMesh(mesh);

	free(data);
	printf("Done!\n");
}
