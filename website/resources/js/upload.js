$(document).ready(function() {
    $("form").on("change", ".file-upload-field", function(event) {
        let file = this.files[0];
        let allowedTypes = ["image/jpeg", "image/jpg", "image/gif", "image/png", "image/bmp"];

        if (file && allowedTypes.includes(file.type)) {
            let fullFilePath = $(this).val();
            let fileName = fullFilePath.replace(/.*(\/|\\)/, '');

            let date = new Date();
            let timestamp = date.getFullYear().toString() +
                            ('0' + (date.getMonth() + 1)).slice(-2) +
                            ('0' + date.getDate()).slice(-2) + "_" +
                            ('0' + date.getHours()).slice(-2) +
                            ('0' + date.getMinutes()).slice(-2) +
                            ('0' + date.getSeconds()).slice(-2);

            let extension = fileName.split('.').pop();
            let generatedFilename = "image_" + timestamp + "." + extension;

            $(this).parent(".file-upload-wrapper").attr("data-text", generatedFilename);

            let formData = new FormData();
            formData.append('file', file);

            $.ajax({
                type: 'POST',
                url: '/uploads',
                data: formData,
                contentType: false,
                processData: false,
                success: function(response) {
                    console.log('File uploaded successfully!');
                },
                error: function(xhr, status, error) {
                    console.log('Error uploading file:', error);
                }
            });
        } else {
            alert("Invalid file type. Only JPEG, JPG, GIF, PNG, and BMP images are allowed.");
            $(this).val(''); 
        }
    });
});