function copyToClipboard(text) {
    navigator.clipboard.writeText(text)
        .then(() => {
            alert("Copied to clipboard!");
        })
        .catch(err => {
            console.error("Failed to copy text: ", err);
            alert("Failed to copy text. Please try again.");
        });
}

document.addEventListener("DOMContentLoaded", () => {
    const copyButtons = document.querySelectorAll(".copy-btn");

    copyButtons.forEach(button => {
        const commandText = button.nextElementSibling.textContent;

        button.addEventListener("click", () => {
            copyToClipboard(commandText);
        });
    });
});


document.addEventListener("DOMContentLoaded", () => {
    const dropdownBtn = document.getElementById("dropdown-btn");
    const dropdownContent = document.getElementById("dropdown-content");
    const btnText = dropdownBtn.querySelector(".btn-text");
    const flagImg = dropdownBtn.querySelector("img");

    // Toggle the dropdown menu on button click
    dropdownBtn.addEventListener("click", (event) => {
        event.stopPropagation(); // Prevent click from bubbling up
        dropdownContent.classList.toggle("show");
    });

    // Handle language selection
    dropdownContent.addEventListener("click", (event) => {
        const selectedOption = event.target.closest("li");
        if (selectedOption) {
            const selectedLang = selectedOption.getAttribute("data-lang");
            const selectedFlag = selectedOption.querySelector("img").src;
            const selectedLangText = selectedOption.textContent.trim();

            // Update the button text and flag
            btnText.textContent = selectedLangText;
            flagImg.src = selectedFlag;

            // Send the selected language to the server via POST
            const formData = new URLSearchParams();
            formData.append("language", selectedLang);

            fetch("/set-language", {
                method: "POST",
                headers: {
                    "Content-Type": "application/x-www-form-urlencoded",
                },
                body: formData.toString(),
            })
                .then((response) => {
                    if (!response.ok) {
                        throw new Error("Failed to save language on the server.");
                    }
                    console.log(`Language "${selectedLang}" saved successfully.`);
                })
                .catch((error) => console.error("Error:", error));

            // Close the dropdown menu
            dropdownContent.classList.remove("show");
        }
    });

    // Close dropdown menu if clicked outside
    document.addEventListener("click", (event) => {
        if (!dropdownBtn.contains(event.target)) {
            dropdownContent.classList.remove("show");
        }
    });
});
