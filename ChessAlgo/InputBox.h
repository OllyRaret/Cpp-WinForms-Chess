#pragma once

namespace ChessAlgo {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для InputBox
	/// </summary>
	public ref class InputBox : public System::Windows::Forms::Form
	{
	public:
		InputBox(String^ name, String^ text, int *value)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
			this->Text = name;
			labelText->Text = text; 
			this->value = value;
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~InputBox()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ labelText;
	private: System::Windows::Forms::Button^ buttonOk;
	protected:

	private: System::Windows::Forms::TextBox^ textBoxValue;


	protected:

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->labelText = (gcnew System::Windows::Forms::Label());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->textBoxValue = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// labelText
			// 
			this->labelText->AutoSize = true;
			this->labelText->Location = System::Drawing::Point(13, 13);
			this->labelText->Name = L"labelText";
			this->labelText->Size = System::Drawing::Size(35, 13);
			this->labelText->TabIndex = 0;
			this->labelText->Text = L"label1";
			// 
			// buttonOk
			// 
			this->buttonOk->Location = System::Drawing::Point(344, 83);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(75, 23);
			this->buttonOk->TabIndex = 1;
			this->buttonOk->Text = L"ОК";
			this->buttonOk->UseVisualStyleBackColor = true;
			this->buttonOk->Click += gcnew System::EventHandler(this, &InputBox::buttonOk_Click);
			// 
			// textBoxValue
			// 
			this->textBoxValue->Location = System::Drawing::Point(12, 57);
			this->textBoxValue->Name = L"textBoxValue";
			this->textBoxValue->Size = System::Drawing::Size(407, 20);
			this->textBoxValue->TabIndex = 2;
			// 
			// InputBox
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(431, 116);
			this->Controls->Add(this->textBoxValue);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->labelText);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"InputBox";
			this->Text = L"InputBox";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		int* value;

	public:
		void changeText(String^ name, String^ text) {
			this->Text = name;
			labelText->Text = text;
		}

	private: 
		System::Void buttonOk_Click(System::Object^ sender, System::EventArgs^ e) {
			if (Int32::TryParse(textBoxValue->Text, *value)) {
				this->Close();
			}
			else {
				MessageBox::Show("Вы ввели не число", "Ошибка", MessageBoxButtons::OK);
			}
		}
	};
}
