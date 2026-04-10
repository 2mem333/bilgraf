	Color TraceRay(Vertex Ro, Vertex Rd, Shape* Shapes[], Vertex camera, int depth, Shape* prevShape)
	{
		if (depth > 4)
		{
			return prevShape->ShapeColor;
			//return Color::Black;
		}

		vector<intersections> Intersections;

		for (int i = 0; i < 16; i++)
		{
			float t = Shapes[i]->Intersect(Ro, Rd);

			if (t > 0.1F)						// 0.1 for avoiding self intersection at reflection
			{
				intersection.distance = t;
				intersection.indice   = i;

				Intersections.push_back(intersection);
			}
		}

		if (Intersections.size() > 0)
		{
			float min_distance = FLT_MAX;
			int   min_indis = -1;

			for (int i = 0; i < Intersections.size(); i++)
			{
				if (Intersections[i].distance < min_distance)
				{
					min_indis    = Intersections[i].indice;
					min_distance = Intersections[i].distance;
				}
			}

			Vertex iPoint = Ro + min_distance * Rd;			
			Shape* S      = Shapes[min_indis];

			TextureMapping(Ro, Rd, S);

			Color reflectedColor;
			if (S->Refl != 0.0F)
			{
				Vertex reflectedDirection = CalculateReflection(S, iPoint, Rd);
				reflectedColor = TraceRay(iPoint, reflectedDirection, Shapes, camera, depth + 1, S);
			}

			Color transmittedColor;
			//mevcut k
			if (S->Trans != 0.0F)
			{
				Vertex transmittedDirection = CalculateTransmission(S, iPoint, Rd);
				transmittedColor = TraceRay(iPoint, transmittedDirection, Shapes, camera, depth + 1, S);
			}

			Color refractedColor;
			if (S->Refrac != 0.0F)
			{
				Vertex refractedDirection = CalculateRefraction(S, iPoint, Rd, 1.0F, 1.33F);
				refractedColor = TraceRay(iPoint, refractedDirection, Shapes, camera, depth + 1, S);
			}

			Color diffuseColor  = ShadeDiffuse(S, iPoint);
			Color specularColor = ShadeSpecular(S, iPoint, camera);

			return ShadingModel(S, diffuseColor, specularColor, reflectedColor, transmittedColor, refractedColor, S->Ambient, S->Dif, S->Spec, S->Refl, S->Trans, S->Refrac);
		}

		return Color::Black;
	}
