fns = glob("*.png");
F = length(fns);
for f=1:F
  img = imread(fns{f});
  iswhite = all(mean(img,3)==255);
  internal = find(~iswhite);
  x0 = internal(1);
  x1 = internal(end);
  img = img(:,x0:x1,:);
  imwrite(img, fns{f});
end
