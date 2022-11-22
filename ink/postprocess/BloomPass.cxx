/**
 * Copyright (C) 2021-2022 Hypertheory
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "BloomPass.h"

namespace Ink {

BloomPass::BloomPass(int w, int h, float t, float i, float r) :
width(w), height(h), threshold(t), intensity(i), radius(r) {}

void BloomPass::init() {
	/* prepare bloom map 1 */
	bloom_map_1 = std::make_unique<Gpu::Texture>();
	bloom_map_1->init_2d(width / 2, height / 2, TEXTURE_R16G16B16_SFLOAT);
	bloom_map_1->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR_MIPMAP_LINEAR);
	bloom_map_1->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	bloom_map_1->generate_mipmap();
	
	/* prepare bloom map 2 */
	bloom_map_2 = std::make_unique<Gpu::Texture>();
	bloom_map_2->init_2d(width / 2, height / 2, TEXTURE_R16G16B16_SFLOAT);
	bloom_map_2->set_filters(TEXTURE_LINEAR, TEXTURE_LINEAR_MIPMAP_LINEAR);
	bloom_map_2->set_wrap_all(TEXTURE_CLAMP_TO_EDGE);
	bloom_map_2->generate_mipmap();
	
	/* prepare bloom render target */
	bloom_target = std::make_unique<Gpu::FrameBuffer>();
	bloom_target->draw_attachments({0});
	
	/* prepare bright pass shader */
	bright_pass_shader = std::make_unique<Gpu::Shader>();
	bright_pass_shader->load_vert_file("ink/shaders/lib/BrightPass.vert.glsl");
	bright_pass_shader->load_frag_file("ink/shaders/lib/BrightPass.frag.glsl");
	
	/* prepare blur shader */
	blur_shader = std::make_unique<Gpu::Shader>();
	blur_shader->load_vert_file("ink/shaders/lib/Blur.vert.glsl");
	blur_shader->load_frag_file("ink/shaders/lib/Blur.frag.glsl");
	
	/* prepare bloom shader */
	bloom_shader = std::make_unique<Gpu::Shader>();
	bloom_shader->load_vert_file("ink/shaders/lib/Bloom.vert.glsl");
	bloom_shader->load_frag_file("ink/shaders/lib/Bloom.frag.glsl");
}

void BloomPass::compile() {
	/* compile bright pass shader */
	bright_pass_shader->compile();
	
	/* update and compile blur shader */
	Defines blur_defines;
	blur_defines.set("BLUR_GAUSSIAN");
	blur_defines.set("TYPE", "vec3");
	blur_defines.set("SWIZZLE", ".xyz");
	blur_shader->set_defines(blur_defines);
	blur_shader->compile();
	
	/* compile bloom shader */
	bloom_shader->compile();
}

void BloomPass::render() const {
	/* change the current viewport */
	Gpu::Rect viewport = RenderPass::get_viewport();
	RenderPass::set_viewport({width / 2, height / 2});
	
	/* 1. render bright pixels to blur map 1 */
	bright_pass_shader->use_program();
	bright_pass_shader->set_uniform_f("threshold", threshold);
	bright_pass_shader->set_uniform_i("map", map->activate(0));
	bloom_target->set_attachment(*bloom_map_1, 0, 0);
	RenderPass::render_to(bright_pass_shader.get(), bloom_target.get());
	
	/* initialize size lod */
	Vec2 size_lod = Vec2(width / 2, height / 2);
	
	/* 2. blur texture on the mipmap chain */
	for (int lod = 0; lod < 5; ++lod) {
		int sigma = lod * 2 + 3;
		
		/* set the viewport of size lod */
		RenderPass::set_viewport(Gpu::Rect(size_lod.x, size_lod.y));
		
		/* blur texture horizontally */
		blur_shader->use_program();
		blur_shader->set_uniform_v2("direction", {1 / size_lod.x, 0});
		blur_shader->set_uniform_f("lod", fmax(0, lod - 1));
		blur_shader->set_uniform_i("radius", sigma * 3);
		blur_shader->set_uniform_f("sigma_s", sigma);
		blur_shader->set_uniform_i("map", bloom_map_1->activate(0));
		bloom_target->set_attachment(*bloom_map_2, 0, lod);
		RenderPass::render_to(blur_shader.get(), bloom_target.get());
		
		/* blur texture vertically */
		blur_shader->use_program();
		blur_shader->set_uniform_v2("direction", {0, 1 / size_lod.y});
		blur_shader->set_uniform_f("lod", lod);
		blur_shader->set_uniform_i("radius", sigma * 3);
		blur_shader->set_uniform_f("sigma_s", sigma);
		blur_shader->set_uniform_i("map", bloom_map_2->activate(0));
		bloom_target->set_attachment(*bloom_map_1, 0, lod);
		RenderPass::render_to(blur_shader.get(), bloom_target.get());
		
		/* update size lod to lower lod */
		size_lod.x = fmax(1, floorf(size_lod.x / 2));
		size_lod.y = fmax(1, floorf(size_lod.y / 2));
	}
	
	/* set back to the initial viewport */
	RenderPass::set_viewport(viewport);
	
	/* 3. blur texture on the mipmap chain */
	bloom_shader->use_program();
	bloom_shader->set_uniform_v3("tint", tint);
	bloom_shader->set_uniform_f("intensity", intensity);
	bloom_shader->set_uniform_f("radius", radius);
	bloom_shader->set_uniform_i("map", map->activate(0));
	bloom_shader->set_uniform_i("bloom_map", bloom_map_1->activate(1));
	RenderPass::render_to(bloom_shader.get(), target);
}

const Gpu::Texture* BloomPass::get_texture() const {
	return map;
}

void BloomPass::set_texture(const Gpu::Texture* t) {
	map = t;
}

}
