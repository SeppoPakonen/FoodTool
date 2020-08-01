#include "CommonCtrl.h"
#include "data.brc"

Image GetSmiley(String s) {
	static VectorMap<String, Image> cache;
	if (cache.IsEmpty()) {
		for(int i = 0; i < smileys_count; i++) {
			String key = smileys_files[i];
			String png = BZ2Decompress(smileys[i], smileys_length[i]);
			cache.Add(key, StreamRaster::LoadStringAny(png));
		}
	}
	int i = cache.Find(s);
	if (i < 0)
		return Image();
	return cache[i];
}

Image GetSmiley(double progress) {
	progress = max(0.0, min(1.0, progress));
	int div = progress * 9;
	switch (div) {
		case 0: return GetSmiley("worst.png");
		case 1: return GetSmiley("verybad.png");
		case 2: return GetSmiley("bad.png");
		case 3: return GetSmiley("stillbad.png");
		case 4: return GetSmiley("halfway.png");
		case 5: return GetSmiley("slightlygood.png");
		case 6: return GetSmiley("good.png");
		case 7: return GetSmiley("verygood.png");
		case 8: return GetSmiley("best.png");
    }
    if (progress < 0) return GetSmiley("worst.png");
    else return GetSmiley("best.png");
}

