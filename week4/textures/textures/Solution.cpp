#include "Common.h"

using namespace std;

bool IsPointInSize(Point p, Size s) {
	return p.x >= 0 && p.x < s.width && p.y >= 0 && p.y < s.height;
}

Size GetImageSize(const Image& image) {
	auto width = static_cast<int>(image.empty() ? 0 : image[0].size());
	auto height = static_cast<int>(image.size());
	return { width, height };
}

class Shape : public IShape {
public:
	void SetPosition(Point pos) override {
		position_ = pos;
	}

	Point GetPosition() const override {
		return position_;
	}

	void SetSize(Size size) override {
		size_ = size;
	}

	Size GetSize() const override {
		return size_;
	}

	void SetTexture(std::shared_ptr<ITexture> texture) override {
		texture_ = move(texture);
	}

	ITexture* GetTexture() const override {
		return texture_.get();
	}

	void Draw(Image& image) const override {
		Point p;
		auto image_size = GetImageSize(image);
		for (p.y = 0; p.y < size_.height; ++p.y) {
			for (p.x = 0; p.x < size_.width; ++p.x) {
				if (IsPointInShape(p)) {
					char pixel = '.';
					if (texture_ && IsPointInSize(p, texture_->GetSize())) {
						pixel = texture_->GetImage()[p.y][p.x];
					}
					Point p1 = { position_.x + p.x, position_.y + p.y };
					if (IsPointInSize(p1, image_size)) {
						image[p1.y][p1.x] = pixel;
					}
				}
			}
		}
	}
private:
	shared_ptr<ITexture> texture_;
	Point position_;
	Size size_;

	virtual bool IsPointInShape(Point) const = 0;
};

class Rectangle : public Shape {
public:
	unique_ptr<IShape> Clone() const override {
		return make_unique<Rectangle>(*this);
	}
private:
	bool IsPointInShape(Point) const override {
		return true;
	}
};

class Ellipse : public Shape {
public:
	unique_ptr<IShape> Clone() const override {
		return make_unique<Ellipse>(*this);
	}
private:
	bool IsPointInShape(Point p) const override {
		return IsPointInEllipse(p, GetSize());
	}
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
	switch (shape_type) {
		case ShapeType::Rectangle:
			return make_unique<Rectangle>();
		case ShapeType::Ellipse:
			return make_unique<Ellipse>();
	}
	return nullptr;
}
