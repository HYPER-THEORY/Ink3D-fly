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

#pragma once

#include "RenderPass.h"

namespace Ink {

class FXAAPass : public RenderPass {
public:
	/**
	 * Creates a new FXAAPass (Fast Approximate Anti-Aliasing) object.
	 */
	explicit FXAAPass() = default;
	
	/**
	 * Initializes the render pass and prepare the resources for rendering.
	 */
	void init() override;
	
	/**
	 * Compiles the required shaders and renders to the render target.
	 */
	void render() const override;
	
	/**
	 * Returns the 2D texture represents the input of rendering pass.
	 */
	const Gpu::Texture* get_texture() const;
	
	/**
	 * Sets the specified 2D texture as the input of rendering pass. Insures the
	 * texture is set to linear filtering.
	 *
	 * \param t input texture
	 */
	void set_texture(const Gpu::Texture* t);
	
private:
	const Gpu::Texture* map = nullptr;
};

}
