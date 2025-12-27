void text(String t, byte x, byte y)
{
  display.setCursor(x, y);
  display.print(t);
}

void REF()
{
  display.clearDisplay();
  display.setTextColor(1);
  text("RUET", 52, 16);
  display.display();
  text("Electra", 43, 32);
  display.display();
  text("Force", 49, 48);
  display.display();

}